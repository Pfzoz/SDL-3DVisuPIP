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

    enum class Projection
    {
        Perspective,
        Orthographic_X,
        Orthographic_Y,
        Orthographic_Z,
        Parallel
    };

    enum class Shading
    {
        Wireframe,
        Constant,
        Gouraud,
        Phong
    };

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
        void use_projection(Projection projection);

        // Getters
        void get_vrp(double *x, double *y, double *z);
        void get_focal_point(double *x, double *y, double *z);
        void get_camera_view_up(double *x, double *y, double *z);
        void get_camera_view_direction(double *x, double *y, double *z);
        void get_camera_view_right(double *x, double *y, double *z);

        // Projection

        // Apply
        SDL_Texture *apply(SDL_Renderer *renderer, SDL_Texture *texture);

        // Acquire Singleton
        static Pipeline &get_pipeline();
    private:
        Projection projection = Projection::Parallel;
        Camera camera;
        Pipeline();
    };

}

#endif
