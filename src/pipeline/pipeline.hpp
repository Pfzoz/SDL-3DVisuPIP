#ifndef PIPELINE_H
#define PIPELINE_H

#include <SDL.h>
#include <vector>
#include <poly.hpp>
#include <utils.hpp>
#include <Eigen/Core>
#include <camera.hpp>

namespace Pip
{
    // Wireframe
    Poly::Polyhedron wireframe(std::vector<SDL_FPoint> generatrix, int slices);

    // Pipeline Singleton
    class Pipeline
    {
    public:
        Poly::Polyhedron scene_objects;
        Camera camera;

        static Pipeline &get_pipeline();
    private:
        Pipeline();
    };

}

#endif
