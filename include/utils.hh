#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include "spin_lattice.hh"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

inline std::vector<std::vector<int>> edge_detection_filter(SpinLattice* lattice)
{
    int N = lattice->get_size();

    // Define a custom ridge kernel
    std::vector<std::vector<int>> ridgeKernel = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};

    int kernelSize = ridgeKernel.size();
    int padding = kernelSize / 2; // Padding size for handling border pixels
    
    std::vector<std::vector<int>> result(N, std::vector<int>(N, 0));

    for (int i = padding; i < N - padding; i++) {
        for (int j = padding; j < N - padding; j++) {
            int sum = 0;

            for (int k = -padding; k <= padding; k++) {
                for (int l = -padding; l <= padding; l++) {
                    sum += (*lattice)(i + k,j + l) * ridgeKernel[k + padding][l + padding];
                }
            }

            result[i][j] = sum;
        }
    }


    return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

inline void loading_bar(int current, int total, int barWidth = 40) {
    float progress = static_cast<float>(current) / total;
    int barLength = static_cast<int>(progress * barWidth);

    std::string green = "\033[1;32m";  
    std::string reset = "\033[0m";

    std::cout << "[";

    for (int i = 0; i < barWidth; ++i) {
        if (i < barLength) {
            std::cout << green << "=" << reset;
        } 
        else if(i == barLength)
        {
            std::cout << green << ">" << reset;
        }
        else
        {
            std::cout << " ";
        }
    }

    std::cout << "] " << static_cast<int>(progress * 100.0) << "% \r";
    std::cout.flush();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

inline void splash_screen()
{
        std::cout << R"(
 ___     _                _____           _                          
|_ _|___(_)_ __   __ _   |  ___|_ _ _ __ | |_ __ _ ___ _   _         
| |/ __| | '_ \ / _`  |  | |_ / _` | '_ \| __/ _` / __| | | |        
| |\__ \ | | | | (_|  |  |  _| (_| | | | | || (_| \__ \ |_| |        
|___|___/_|_| |_|\__, |  |_|  \__,_|_| |_|\__\__,_|___/\__, |        
                  |___/                                 |___/         
 __  __                ____                           _             
|  \/  | __ _ _ __    / ___| ___ _ __   ___ _ __ __ _| |_ ___  _ __ 
| |\/| |/ _` | '_ \  | |  _ / _ \ '_ \ / _ \ '__/ _` | __/ _ \| '__|
| |  | | (_| | |_) | | |_| |  __/ | | |  __/ | | (_| | || (_) | |   
|_|  |_|\__,_| .__/   \____|\___|_| |_|\___|_|  \__,_|\__\___/|_|   
             |_|                                                    

-------------------------------------------------------------------------------- 
version : 1.0
--------------------------------------------------------------------------------                       
    )" << std::endl;


    return ;
}

#endif