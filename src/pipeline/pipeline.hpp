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
        PERSPECTIVE,
        ORTHOGRAPHIC_X,
        ORTHOGRAPHIC_Y,
        ORTHOGRAPHIC_Z,
        PARALLEL
    };

    enum class Shading
    {
        NO_SHADING,
        CONSTANT,
        GOURAUD,
        PHONG,
    };

    // Wireframe
    Poly::Polyhedron wireframe(std::vector<SDL_FPoint> generatrix, int slices);

    // Pipeline Singleton
    class Pipeline
    {
    private:
        Projection projection = Projection::PARALLEL;
        Shading shading = Shading::NO_SHADING;
        Camera camera;
        SDL_Rect window;
    public:
        std::vector<Poly::Polyhedron> scene_objects;

        // Setters
        void set_vrp(double x, double y, double z);
        void set_focal_point(double x, double y, double z);
        void use_projection(Projection projection_mode);
        void use_shading(Shading shading_mode);
        void set_window(SDL_Rect dimensions);

        // Getters
        void get_vrp(double *x, double *y, double *z);
        void get_focal_point(double *x, double *y, double *z);
        void get_camera_view_up(double *x, double *y, double *z);
        void get_camera_view_direction(double *x, double *y, double *z);
        void get_camera_view_right(double *x, double *y, double *z);
        SDL_Rect get_window();
        Projection get_projection();
        Shading get_shading();

        // Objects
        void add_object(Poly::Polyhedron object);

        // Projection
        

        // Apply
        void apply(SDL_Renderer *renderer, SDL_Texture *texture);

        // Acquire Singleton
        static Pipeline &get_pipeline();
    private:
        Pipeline();

        // WV Matrix
        Eigen::Matrix4d window_to_viewport_matrix();

        // Projections
        Eigen::Matrix4d ortographic_x_matrix();
        Eigen::Matrix4d ortographic_y_matrix();
        Eigen::Matrix4d ortographic_z_matrix();
        Eigen::Matrix4d parallel_matrix();
        Eigen::Matrix4d perspective_matrix();

    };

}

#endif
