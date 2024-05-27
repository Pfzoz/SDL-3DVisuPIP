#ifndef PIPELINE_H
#define PIPELINE_H

#include <SDL.h>
#include <vector>
#include <poly.hpp>
#include <utils.hpp>
#include <Eigen/Core>
#include <camera.hpp>
#include <wireframe.hpp>

namespace Scene
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
    Poly::Polyhedron create_wireframe(std::vector<SDL_FPoint> generatrix, int slices);

    // Pipeline Singleton
    class Pipeline
    {
    private:
        bool use_painter_clipper = false;
        double projection_distance = 1.0f;
        double ambient_light_intensity_r = 0.1f;
        double ambient_light_intensity_g = 0.1f;
        double ambient_light_intensity_b = 0.1f;
        double illumination_intensity_r = 0.5f;
        double illumination_intensity_g = 0.5f;
        double illumination_intensity_b = 0.5f;

        Projection projection = Projection::PARALLEL;
        Shading shading = Shading::NO_SHADING;
        Camera camera;
        SDL_Rect window, screen;
        SDL_Texture *z_buffer_cache = NULL;
        Eigen::MatrixXd z_buffer;
        Eigen::MatrixXi color_buffer;
        Eigen::Vector3d lights_position = Eigen::Vector3d(0.0f, 0.0f, 1.0f);
        Eigen::Vector3d src_lights_position;
        std::vector<std::vector<double[3]>> gouraud_illuminations;

    public:
        std::vector<Poly::Polyhedron> scene_objects;

        // Setters
        void set_vrp(double x, double y, double z);
        void set_focal_point(double x, double y, double z);
        void rotate_camera(double x, double y, double z);
        void translate_camera(double x, double y, double z);
        void set_projection_distance(double distance);
        void use_projection(Projection projection_mode);
        void use_shading(Shading shading_mode);
        void use_painter_clip(bool use_painter_clipper);
        void set_window(SDL_Rect dimensions);
        void set_srt(SDL_Rect dimensions);
        void set_ambient_light(double r, double g, double b);
        void set_lights_intensity(double r, double g, double b);
        void set_lights_position(double x, double y, double z);

        // Getters
        void get_vrp(double *x, double *y, double *z);
        void get_focal_point(double *x, double *y, double *z);
        void get_camera_view_up(double *x, double *y, double *z);
        void get_camera_view_direction(double *x, double *y, double *z);
        void get_camera_view_right(double *x, double *y, double *z);
        double get_projection_distance();
        void get_ambient_light_intensity(double *r, double *g, double *b);
        void get_illumination_intensity(double *r, double *g, double *b);
        void get_lights_position(double *x, double *y, double *z);
        SDL_Rect get_window();
        SDL_Rect get_srt();
        Projection get_projection();
        Shading get_shading();
        bool using_painter_clipper();
        bool is_altered();

        // Objects
        void get_object_center(int, double *x, double *y, double *z);
        void add_object(Poly::Polyhedron object);
        void remove_object(int);
        void translate_object(int, double x, double y, double z);
        void scale_object(int, double x, double y, double z);
        void rotate_object(int, double x, double y, double z);
        uint get_object_color(int);
        void get_object_ambient_coefficients(int, double *x, double *y, double *z);
        void get_object_diffuse_coefficients(int, double *x, double *y, double *z);
        void get_object_specular_coefficients(int, double *x, double *y, double *z);
        double get_object_specular_exponent(int);
        void set_object_specular_exponent(int, double n);
        void set_object_ambient_coefficients(int, double x, double y, double z);
        void set_object_diffuse_coefficients(int, double x, double y, double z);
        void set_object_specular_coefficients(int, double x, double y, double z);
        // Render
        void render(SDL_Renderer *renderer, SDL_Window *window);

        // Acquire Singleton
        static Pipeline &get_pipeline();

    private:
        Pipeline();

        bool pipeline_altered = true;

        // WV Matrix
        Eigen::Matrix4d window_to_viewport_matrix();

        // Projections
        Eigen::Matrix4d ortographic_x_matrix();
        Eigen::Matrix4d ortographic_y_matrix();
        Eigen::Matrix4d ortographic_z_matrix();
        Eigen::Matrix4d parallel_matrix();
        Eigen::Matrix4d perspective_matrix();

        // Painter Algorithm
        void apply_painter_clipper(std::vector<Poly::Polyhedron> &polyhedra);

        // Lights
        uint apply_lights(Poly::Polyhedron &poly, int face_index);

        // Projection
        Eigen::Matrix4d get_projection_matrix();

        // Shading
        void apply_shading(std::vector<Poly::Polyhedron> &polyhedra, SDL_Renderer *renderer);
        void apply_constant_shading_to_polyhedron(Poly::Polyhedron poly, Eigen::MatrixXd &z_buffer, Eigen::MatrixXi &color_buffer);
        void apply_constant_shading(std::vector<Poly::Polyhedron> &polyhedra, SDL_Renderer *renderer);
        void apply_gouraud_shading_to_polyhedron(Poly::Polyhedron poly, Eigen::MatrixXd &z_buffer, Eigen::MatrixXi &color_buffer);
        void apply_gouraud_shading(std::vector<Poly::Polyhedron> &polyhedra, SDL_Renderer *renderer);
        void apply_phong_shading_to_polyhedron(Poly::Polyhedron poly, Eigen::MatrixXd &z_buffer, Eigen::MatrixXi &color_buffer);
        void apply_phong_shading(std::vector<Poly::Polyhedron> &polyhedra, SDL_Renderer *renderer);
        void apply_wireframe_shading(std::vector<Poly::Polyhedron> polyhedra, SDL_Renderer *renderer);

        void get_gouraud_illuminations(std::vector<Poly::Polyhedron> polyhedra);
    };

}

#endif
