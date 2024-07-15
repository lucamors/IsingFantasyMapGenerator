#include "map.hh"
#include "utils.hh"
#include "spin_lattice.hh"
#include <cmath>
#include <limits>
#include <random>
#include <cstdlib>
#include <algorithm>
#include <iostream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Map::Map(SpinLattice* lattice)
{
    N = lattice->get_size();

    // Init heightmap using the lattice dimension
    init_heightmap(N);


    // Apply edge detection to the lattice
    std::vector<std::vector<int>> edge_detected_grid = edge_detection_filter(lattice);

    // Extract border cells
    std::vector<cell> borders;

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(edge_detected_grid[i][j] < 0){
                borders.push_back({i,j,0});
            }
        }
    }
    
    // Compute the heightmap 
    maxval = std::numeric_limits<float>::min();
    minval = std::numeric_limits<float>::max();

    for(int i = 0; i < N; i++)
    {
        loading_bar(i+1,N);

        for(int j = 0; j < N; j++)
        {
            float minimum_distance = 1000000;
           

            for(auto & borderpixel : borders )
            {
                float dist = sqrt((borderpixel.i-i)*(borderpixel.i-i)+(borderpixel.j-j)*(borderpixel.j-j));

                if(dist < minimum_distance)
                {
                    minimum_distance = dist;
                }

            }
            
            heightmap[i][j] =  (*lattice)(i,j) == 1 ? minimum_distance : -minimum_distance;

            // Search for bounds
            if(heightmap[i][j] > maxval)
            {
                maxval = heightmap[i][j];
            }

            if(heightmap[i][j] < minval)
            {
                minval = heightmap[i][j];
            }
            
        } 
    }

    

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Map::~Map(){

    for (size_t i = 0; i < N; i++)
    {
        delete[] heightmap[i];
    }

    delete[] heightmap;
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

int& Map::operator()(int i, int j) 
{
    return heightmap[i][j];
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

float Map::get_min_height()
{
    return minval;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

int Map::get_map_dimension()
{
    return N;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

float Map::get_max_height()
{
    return maxval;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void Map::init_heightmap(int N)
{

    heightmap = new int*[N];
    for (auto i = 0; i < N; i++)
    {
        heightmap[i] = new int[N];
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
 
            // Assign values to the
            // memory blocks created
            heightmap[i][j] = -1;
        }
    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool Map::point_inside(int x, int y, std::vector<point> points, float ref_distance)
{
    float dist;
    for(auto a = 0; a < points.size(); a++)
    {
        dist = sqrt((points[a].x-x)*(points[a].x-x) + (points[a].y-y)*(points[a].y-y));
        if(dist < ref_distance){ return true; }
    }

    return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


void Map::generate_river()
{

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, N-1);

    int start_x = 0, start_y = 0;
    int failed_attempts = 0;

    while(heightmap[start_x][start_y] < maxval/3 or point_inside(start_x,start_y,river_starting_points,30))
    {
        start_x = distribution(gen);
        start_y = distribution(gen);
        failed_attempts++;
        if(failed_attempts > 1000)
        {
            return ;
        }
    }

    river_starting_points.push_back({start_x,start_y});

    float ch = heightmap[start_x][start_y];
    while(ch > 0)
    {
        std::vector<int> suitable_x;
        std::vector<int> suitable_y;

        if(heightmap[start_x+1][start_y] <= ch)
        {
            suitable_x.push_back(start_x+1);
            suitable_y.push_back(start_y);
            
        }
        
        if(heightmap[start_x][start_y+1] <= ch)
        {
            suitable_x.push_back(start_x);
            suitable_y.push_back(start_y+1);
            
        }
        
        if(heightmap[start_x-1][start_y] <= ch)
        {
            suitable_x.push_back(start_x-1);
            suitable_y.push_back(start_y);
            
        }
        
        if(heightmap[start_x][start_y-1] <= ch)
        {
            suitable_x.push_back(start_x);
            suitable_y.push_back(start_y-1);
            
        }

        if(suitable_x.size()==0)
        {
            return;
        }

        std::uniform_int_distribution<int> distint(0,suitable_x.size()-1);

        int chosen_index = distint(gen);
        
        start_x = suitable_x[chosen_index];
        start_y = suitable_y[chosen_index];

        ch = heightmap[start_x][start_y];
        heightmap[start_x][start_y] = 1000;
        
    }
    

    return ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void Map::generate_tree()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, N-1);

    int start_x = 0, start_y = 0;

    int failed_attempts = 0;

    while(heightmap[start_x][start_y] > maxval*0.35 or heightmap[start_x][start_y] < maxval*0.2 or point_inside(start_x,start_y,tree_starting_points,4) or point_inside(start_x,start_y,mountain_starting_points,15) )
    {
        start_x = distribution(gen);
        start_y = distribution(gen);
    
        failed_attempts++;
        if(failed_attempts > 1000)
        {
            return ;
        }
    }


    tree_starting_points.push_back({start_x,start_y});

    heightmap[start_x][start_y] = 1001;
    
    heightmap[start_x][start_y-1] = 1001;
    heightmap[start_x-1][start_y-1] = 1001;
    heightmap[start_x+2][start_y-1] = 1001;
    heightmap[start_x-2][start_y-1] = 1001;
    heightmap[start_x+1][start_y-1] = 1001;

    heightmap[start_x][start_y-2] = 1001;
    heightmap[start_x-1][start_y-2] = 1001;
    heightmap[start_x+1][start_y-2] = 1001;

    heightmap[start_x][start_y-3] = 1001;
    


    return ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


void Map::generate_mountain()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, N-1);

    int start_x = 0, start_y = 0;

    int failed_attempts = 0;

    while((heightmap[start_x][start_y] < maxval*0.45 or point_inside(start_x,start_y,mountain_starting_points,8) or heightmap[start_x][start_y] > 600 ))
    {
        if(heightmap[start_x][start_y] == 1000 or heightmap[start_x][start_y] == 1001 or heightmap[start_x][start_y] == 1002) 
        {
            failed_attempts++;
            if(failed_attempts > 100)
            {
                std::cout << ",,," << std::endl;
                return ;
            }
            
        }

        start_x = distribution(gen);
        start_y = distribution(gen);


        failed_attempts++;
        if(failed_attempts > 100)
        {
            return ;
        }
    }

    mountain_starting_points.push_back({start_x,start_y});

    heightmap[start_x][start_y] = 1001;
    heightmap[start_x+1][start_y] = 1001;
    heightmap[start_x+8][start_y] = 1001;
    heightmap[start_x+9][start_y] = 1001;

    heightmap[start_x+1][start_y-1] = 1001;
    heightmap[start_x+2][start_y-1] = 1001;
    heightmap[start_x+7][start_y-1] = 1001;
    heightmap[start_x+8][start_y-1] = 1001;

    heightmap[start_x+2][start_y-2] = 1001;
    heightmap[start_x+6][start_y-2] = 1001;
    heightmap[start_x+7][start_y-2] = 1001;
    heightmap[start_x+8][start_y-2] = 1001;

    heightmap[start_x+2][start_y-3] = 1001;
    heightmap[start_x+3][start_y-3] = 1001;
    heightmap[start_x+5][start_y-3] = 1001;    
    heightmap[start_x+6][start_y-3] = 1001;
    heightmap[start_x+7][start_y-3] = 1001;

    heightmap[start_x+3][start_y-4] = 1001;
    heightmap[start_x+4][start_y-4] = 1001;
    heightmap[start_x+5][start_y-4] = 1001;
    heightmap[start_x+6][start_y-4] = 1001;

    heightmap[start_x+4][start_y-5] = 1001;
    heightmap[start_x+5][start_y-5] = 1001;
    heightmap[start_x+6][start_y-5] = 1001;

    heightmap[start_x+5][start_y-6] = 1001;

    return ;
}