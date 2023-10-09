#ifndef MAP_H
#define MAP_H

#include "spin_lattice.hh"
#include <vector>

typedef struct cell{
    int i;
    int j;
    float d;
} cell;

typedef struct point{
    int x;
    int y;
} point;

class Map{

    public:

        Map(SpinLattice*);
        ~Map();

        int& operator()(int i, int j);

        float get_min_height();
        float get_max_height();
        int get_map_dimension();

        void generate_river();
        void generate_tree();
        void generate_mountain();

    private:

        void init_heightmap(int);
        bool point_inside(int, int , std::vector<point>, float);

        int N;
        float minval;
        float maxval;

        int** heightmap;

        std::vector<point> river_starting_points;
        std::vector<point> tree_starting_points;
        std::vector<point> mountain_starting_points;


};

#endif