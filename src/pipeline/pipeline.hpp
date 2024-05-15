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

        // Setters
        void set_vrp(double x, double y, double z);
        void set_focal_point(double x, double y, double z);
        
        // Getters
        void get_vrp(double *x, double *y, double *z);
        void get_focal_point(double *x, double *y, double *z);
        void get_camera_view_up(double *x, double *y, double *z);
        void get_camera_view_direction(double *x, double *y, double *z);
        void get_camera_view_right(double *x, double *y, double *z);

        // Acquire Singleton
        static Pipeline &get_pipeline();
    private:
        Camera camera;
        Pipeline();
    };

}

#endif
