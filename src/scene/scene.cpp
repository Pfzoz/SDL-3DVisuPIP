#include "scene.hpp"

Scene::Pipeline::Pipeline() {}

Scene::Pipeline &Scene::Pipeline::get_pipeline()
{
    static Pipeline instance;
    return instance;
}

// Setters
void Scene::Pipeline::set_vrp(double x, double y, double z)
{
    this->camera.set_vrp({x, y, z});
    this->pipeline_altered = true;
}

void Scene::Pipeline::set_focal_point(double x, double y, double z)
{
    this->camera.set_focal_point({x, y, z});
    this->pipeline_altered = true;
}

void Scene::Pipeline::set_window(SDL_Rect dimensions)
{
    this->window = dimensions;
    this->pipeline_altered = true;
}

void Scene::Pipeline::set_srt(SDL_Rect dimensions)
{
    this->screen = dimensions;
    this->pipeline_altered = true;
}

void Scene::Pipeline::use_projection(Projection projection_mode)
{
    this->projection = projection_mode;
    this->pipeline_altered = true;
}

// Getters
void Scene::Pipeline::get_vrp(double *x, double *y, double *z)
{
    Eigen::Vector3d vrp = this->camera.get_vrp();
    *x = vrp(0);
    *y = vrp(1);
    *z = vrp(2);
}

void Scene::Pipeline::get_focal_point(double *x, double *y, double *z)
{
    Eigen::Vector3d focal_point = this->camera.get_focal_point();
    *x = focal_point(0);
    *y = focal_point(1);
    *z = focal_point(2);
}

void Scene::Pipeline::get_camera_view_direction(double *x, double *y, double *z)
{
    Eigen::Vector3d n = this->camera.get_n();
    *x = n(0);
    *y = n(1);
    *z = n(2);
}

void Scene::Pipeline::get_camera_view_right(double *x, double *y, double *z)
{
    Eigen::Vector3d u = this->camera.get_u();
    *x = u(0);
    *y = u(1);
    *z = u(2);
}

void Scene::Pipeline::get_camera_view_up(double *x, double *y, double *z)
{
    Eigen::Vector3d v = this->camera.get_v();
    *x = v(0);
    *y = v(1);
    *z = v(2);
}

SDL_Rect Scene::Pipeline::get_srt()
{
    return this->screen;
}

bool Scene::Pipeline::is_altered()
{
    return this->pipeline_altered;
}

// Window to Viewport
Eigen::Matrix4d Scene::Pipeline::window_to_viewport_matrix()
{
    // U = Screen X, V = Screen Y, X = Window X, Y = Window Y
    Eigen::Matrix4d matrix;
    matrix << 1, 0, 0, 1,
        0, 1, 0, 1,
        0, 0, 1, 0,
        0, 0, 0, 1;
    double screen_range_x = screen.w - screen.x;
    double screen_range_y = screen.h - screen.y;
    double window_range_x = window.w - window.x;
    double window_range_y = window.h - window.y;
    matrix(0, 0) = (screen_range_x) / (window_range_x);
    matrix(0, 3) = -window.x * (screen_range_x / window_range_x) + screen.x;
    matrix(1, 1) = (screen_range_y) / (window_range_y);
    matrix(1, 3) = -window.y * (screen_range_y / window_range_y) + screen.y;
    return matrix;
}

// Projections
Eigen::Matrix4d Scene::Pipeline::get_projection_matrix()
{
    Eigen::Matrix4d return_matrix;
    switch (projection)
    {
    case Projection::PERSPECTIVE:
        return perspective_matrix();
        break;
    case Projection::ORTHOGRAPHIC_X:
        // return ortographic_x_matrix();
        break;
    case Projection::ORTHOGRAPHIC_Y:
        // return ortographic_y_matrix();
        break;
    case Projection::ORTHOGRAPHIC_Z:
        // return ortographic_z_matrix();
        break;
    case Projection::PARALLEL:
        return parallel_matrix();
        break;
    default:
        printf("No projection defined!\n");
        std::exit(1);
    }
    return return_matrix;
}

// Parallel
Eigen::Matrix4d Scene::Pipeline::parallel_matrix()
{
    Eigen::Matrix4d matrix;
    matrix << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 1;
    return matrix;
}

// Perspective

Eigen::Matrix4d Scene::Pipeline::perspective_matrix()
{
    Eigen::Matrix4d matrix;
    // Zprp = VRP | Zvp = Focal Point
    double zprp = 0;
    double zvp = camera.get_focal_point().norm();
    double dp_distance = (zvp - zprp);
    matrix << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, ((-zvp)/dp_distance), zvp*((zprp/dp_distance)),
        0, 0, -(1/dp_distance), (zprp/dp_distance);
    return matrix;
}

// Shading
void Scene::Pipeline::apply_shading(std::vector<Poly::Polyhedron> polyhedra, SDL_Renderer *renderer)
{
    switch (shading)
    {
    case Shading::NO_SHADING:
        apply_wireframe_shading(polyhedra, renderer);
        break;
    case Shading::CONSTANT:
        break;
    case Shading::GOURAUD:
        break;
    case Shading::PHONG:
        break;
    default:
        printf("No shading defined!\n");
        std::exit(1);
    }
}

void Scene::Pipeline::apply_wireframe_shading(std::vector<Poly::Polyhedron> polyhedra, SDL_Renderer *renderer)
{
    for (int i = 0; i < polyhedra.size(); i++)
    {
        for (int j = 0; j < polyhedra[i].segments.size(); j++)
        {
            Eigen::Vector3d v1 = polyhedra[i].vertices[polyhedra[i].segments[j].p1];
            Eigen::Vector3d v2 = polyhedra[i].vertices[polyhedra[i].segments[j].p2];
            SDL_FPoint p1 = {(float)v1.x(), (float)v1.y()};
            SDL_FPoint p2 = {(float)v2.x(), (float)v2.y()};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
        }
    }
}

// Objects
void Scene::Pipeline::get_object_center(size_t index, double *x, double *y, double *z)
{
    Eigen::Vector3d center = scene_objects[index].get_center();
    *x = center(0);
    *y = center(1);
    *z = center(2);
}

void Scene::Pipeline::add_object(Poly::Polyhedron object)
{
    scene_objects.push_back(object);
    this->pipeline_altered = true;
}

void Scene::Pipeline::remove_object(size_t index)
{
    scene_objects.erase(scene_objects.begin() + index);
    this->pipeline_altered = true;
}

void Scene::Pipeline::translate_object(size_t index, double x, double y, double z)
{
    scene_objects[index].translate(x, y, z);
    this->pipeline_altered = true;
}

void Scene::Pipeline::rotate_object(size_t index, double x, double y, double z)
{
    scene_objects[index].rotate(x, y, z);
    this->pipeline_altered = true;
}

// Z-Buffer

void Scene::Pipeline::apply_z_buffer(std::vector<Poly::Polyhedron> &polyhedra)
{
    // TODO
}

// Pipeline Main Flux

void Scene::Pipeline::render(SDL_Renderer *renderer)
{
    Eigen::Matrix4d wv_matrix = this->window_to_viewport_matrix();
    Eigen::Matrix4d projection_matrix = this->get_projection_matrix();
    Eigen::Matrix4d src_matrix = this->camera.get_src_matrix();
    Eigen::Matrix4d sru_srt_matrix = wv_matrix * projection_matrix * src_matrix;
    std::vector<Poly::Polyhedron> polyhedra = this->scene_objects;
    std::vector<std::vector<double>> h_factors(polyhedra.size());
    for (int i = 0; i < polyhedra.size(); i++)
        polyhedra[i].transform(sru_srt_matrix, &h_factors[i]);
    if (projection == Projection::PERSPECTIVE)
    {
        for (int i = 0; i < polyhedra.size(); i++)
        {
            for (int j = 0; j < polyhedra[i].vertices.size(); j++)
                polyhedra[i].vertices[j] /= h_factors[i][j];
;        }
    }
    if (use_z_buffer)
        apply_z_buffer(polyhedra);
    apply_shading(polyhedra, renderer);
    this->pipeline_altered = false;
}

// Wireframe

Poly::Polyhedron Scene::create_wireframe(std::vector<SDL_FPoint> generatrix, int slices)
{
    return wireframe(generatrix, slices);
}