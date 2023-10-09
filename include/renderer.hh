#ifndef RENDERER_H
#define RENDERER_H

#include "map.hh"
#include <string>

typedef struct RGB{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    
} RGB;

class Renderer{

    public:

        Renderer();
        ~Renderer();

        void render(Map *, std::string);

    private:

        RGB get_color(int);

        float minval;
        float maxval;
};

#endif