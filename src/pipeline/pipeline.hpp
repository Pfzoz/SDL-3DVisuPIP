#ifndef PIPELINE_H
#define PIPELINE_H

#include <SDL2/SDL.h>
#include <vector>
#include "../poly/poly.hpp"

namespace Pipeline
{
    // Wireframe
    Poly::Polyhedron wireframe(std::vector<SDL_FPoint> generatrix, int slices);

    // Pipeline Singleton
    class Pipeline
    {
    public:
        static Pipeline get_pipeline();
    private:
        Pipeline *instance = nullptr;
        Pipeline();
    };

}

#endif