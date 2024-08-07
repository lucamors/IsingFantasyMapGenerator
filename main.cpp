#include <string>
#include <iostream>
#include <unistd.h>
#include "utils.hh"
#include "map.hh"
#include "renderer.hh"
#include "spin_lattice.hh"

int main(int argc, char* argv[]) {

    // Parsing arguments
    int N = 0; // number of images to be generated
    int L = 0; // lattice size (LxL)

     for (int i = 1; i < argc; i++) 
     {
        std::string arg = argv[i];

        if (arg == "--num-generated-images" || arg == "-N") 
        {
            
            if (i + 1 < argc) 
            {
                N = std::stoi(argv[i + 1]);
                i++;  // Skip the next argument
            } 
            else 
            {
                std::cerr << "Error: --num-generated-images option requires an argument." << std::endl;
                return 1;
            }
        }
        else if (arg == "--lattice-size" || arg == "-L") 
        {
            
            if (i + 1 < argc) 
            {
                L = std::stoi(argv[i + 1]);
                i++;  // Skip the next argument
            } 
            else 
            {
                std::cerr << "Error: --lattice-size option requires an argument." << std::endl;
                return 1;
            }
        }
        else 
        {
            std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
            return 1;
        }
    }

    if(N==0) 
    {
        std::cerr << "Error: option --num-generated-images is required" << std::endl;
        exit(1);
    }
    if(L==0)
    {
        std::cerr << "Error: option --lattice-size is required" << std::endl;
        exit(1);
    }
    
    // Print program name and version
    splash_screen();

    for(int n = 1; n <= N; n++)
    {
        // Declare a spin lattice object 
        SpinLattice lattice(L);

        // Annealing procedure
        std::cout << std::endl << "Starting annealing procedure for map #" << n << std::endl;
        lattice.anneal(100000);
        
        // Remove noisy pixel by "freezing" the lattice
        std::cout << std::endl << "Freezing the lattice: " << std::endl;
        lattice.freeze();

        // declare a map object starting from the sea/terrain pattern
        // generated by the Ising Lattice
        std::cout << std::endl << "Generating heightmap from spin lattice:" << std::endl;
        Map map(lattice);

        // generating amenities on map using the heightmap
        // Generate a set of rivers
        std::cout << std::endl << "Generating rivers . . ." << std::endl;
        for (size_t i = 0; i < 20; i++)
        {
            map.generate_river();
        }

        std::cout << "Generating mountains . . ." << std::endl;
        for (size_t i = 0; i < 150; i++)
        {
            map.generate_mountain();
        }
        

        std::cout << "Generating trees . . ." << std::endl;
        for (size_t i = 0; i < 700; i++)
        {
            map.generate_tree();
        }

        // Render the map
        std::cout << "Rendering the map . . ." << std::endl;
        Renderer renderer = Renderer();

        std::string filename = "map"+std::to_string(n)+".bmp";
        renderer.render(&map, filename);

        char buffer[200];
        getcwd(buffer, sizeof(buffer));
        
        std::cout << "Map saved into " << buffer << "/" << filename << std::endl;

    }
    return 0;
}
