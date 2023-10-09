#include "renderer.hh"
#include "map.hh"
#include <string>
#include <SDL2/SDL.h>
#include <iostream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Renderer::Renderer(){}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Renderer::~Renderer(){}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

RGB Renderer::get_color(int value)
{

    if (value == 1000) return {250, 223, 165};
    if (value == 1001) return {87, 60, 6};
    if (value == 1002) return {237, 222, 161};

    if (value > maxval) value = maxval;

    RGB color;

    // Define RGB values for yellow, green, and brown
    RGB yellow = {165,116,57};
    RGB orange = {183,143,81};
    RGB brown = {214,185,117};

    RGB coastal_yellow = {237,213,151};
    RGB deep_yellow = {202,168,96};

    if (value <= 0){

        float valued = (1/(minval*minval))*(((float)value) - minval)*(((float)value) - minval);       
        float factor2 = valued;
        float factor1 = 1.0 - factor2;
        if (factor1 > 1) factor2 =0;
        // Interpolate between the colors
        color.r = static_cast<int>(deep_yellow.r * factor1 + coastal_yellow.r * factor2);
        color.g = static_cast<int>(deep_yellow.g * factor1 + coastal_yellow.g * factor2);
        color.b = static_cast<int>(deep_yellow.b * factor1 + coastal_yellow.b * factor2);
        return color;
    }

    // Calculate the interpolation factors
    
    // std::cout << factor1 << " " << factor2 << std::endl;
    if(value < maxval/3){

        float factor1 = (float)(value) / (maxval/3);
        float factor2 = 1.0 - factor1;

        // Interpolate between the colors
        color.r = static_cast<int>(yellow.r * factor2 + orange.r * factor1);
        color.g = static_cast<int>(yellow.g * factor2 + orange.g * factor1);
        color.b = static_cast<int>(yellow.b * factor2 + orange.b * factor1);
    }
    else if(value >= maxval/3){
        float factor1 = (float)(value-(maxval*0.33)) / (maxval*2/3);
        float factor2 = 1.0 - factor1;
        // std::cout << value << " " << maxval << " " << factor1 << " " << factor2 << std::endl;
        // Interpolate between the colors
        color.r = static_cast<int>(orange.r * factor2 + brown.r * factor1);
        color.g = static_cast<int>(orange.g * factor2 + brown.g * factor1);
        color.b = static_cast<int>(orange.b * factor2 + brown.b * factor1);
    }

    return color;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void Renderer::render(Map * map, std::string filename)
{
    minval = map->get_min_height();
    maxval = map->get_max_height();
    
    int cell_size = 5;
    int N= map->get_map_dimension();
    SDL_Surface* surface = SDL_CreateRGBSurface(0, N * cell_size, N * cell_size, 32, 0, 0, 0, 0);

    if (!surface) {
        std::cerr << "SDL_CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        return;
    }

    RGB sea = {0,0,255};
    RGB terrain = {255, 171, 25};

    for (int x = 0; x < N; ++x) {
        for (int y = 0; y < N; ++y) {
            SDL_Rect rect = {x * cell_size, y * cell_size, cell_size, cell_size};
            
            
            RGB tile = get_color((*map)(x,y));

            Uint32 color = SDL_MapRGB(surface->format, tile.r, tile.g, tile.b);
            SDL_FillRect(surface, &rect, color);
        }
    }

    if (SDL_SaveBMP(surface, filename.c_str()) != 0) {
        std::cerr << "SDL_SaveBMP failed: " << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(surface);

    return ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
