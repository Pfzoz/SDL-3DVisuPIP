#include "scene.hpp"

struct Scanline_Edge
{
    double min_y, max_y;
    double min_y_x, max_y_x;
    double min_y_z, max_y_z;
    double x_y_delta, z_y_delta;
    double slope;
};

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
    double zvp = (camera.get_focal_point() - camera.get_vrp()).norm();
    double dp_distance = (zvp - zprp);
    matrix << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, ((-zvp) / dp_distance), zvp * ((zprp / dp_distance)),
        0, 0, -(1 / dp_distance), (zprp / dp_distance);
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
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, static_cast<int>(v1.x()), static_cast<int>(v1.y()),
                               static_cast<int>(v2.x()), static_cast<int>(v2.y()));
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

bool Scanline_Edge_Comparison(Scanline_Edge a, Scanline_Edge b)
{
    return a.min_y < b.min_y;
}

void apply_z_buffer_to_polyhedron(Poly::Polyhedron poly, Eigen::MatrixXd &z_buffer, Eigen::MatrixXi &color_buffer)
{
    for (size_t i = 0; i < poly.faces.size(); i++)
    {
        std::vector<Scanline_Edge> scanline_edges;
        Poly::Segment v_seg = poly.segments[poly.faces[i].segments[0]];
        Poly::Segment u_seg = poly.segments[poly.faces[i].segments[1]];
        Poly::Segment e_seg = poly.segments[poly.faces[i].segments[2]];
        Eigen::Vector3d v = poly.vertices[v_seg.p1] - poly.vertices[v_seg.p2];
        Eigen::Vector3d u = poly.vertices[u_seg.p1] - poly.vertices[u_seg.p2];
        if (v.isApprox(u))
            u = poly.vertices[e_seg.p1] - poly.vertices[e_seg.p2];
        Eigen::Vector3d n = v.cross(u);
        double a = n(0), c = n(2);
        double lowest_y = std::numeric_limits<double>::max();
        double highest_y = std::numeric_limits<double>::lowest();
        for (size_t seg_j : poly.faces[i].segments)
        {
            Scanline_Edge edge;
            Eigen::Vector3d p1, p2;
            p1 = poly.vertices[poly.segments[seg_j].p1];
            p2 = poly.vertices[poly.segments[seg_j].p2];
            if (p1.y() == p2.y())
                continue;
            if (p1.y() < p2.y())
                std::swap(p1, p2);
            edge.max_y = p1.y();
            edge.max_y_x = p1.x();
            edge.max_y_z = p1.z();
            edge.min_y = p2.y();
            edge.min_y_x = p2.x();
            edge.min_y_z = p2.z();
            if (edge.min_y < lowest_y)
                lowest_y = edge.min_y;
            if (edge.max_y > highest_y)
                highest_y = edge.max_y;
            edge.x_y_delta = ((double)edge.max_y_x - (double)edge.min_y_x) / ((double)edge.max_y - (double)edge.min_y);
            edge.z_y_delta = ((double)edge.max_y_z - (double)edge.min_y_z) / ((double)edge.max_y - (double)edge.min_y);
            edge.slope = ((double)edge.max_y_z - (double)edge.min_y_z) / ((double)edge.max_y_x - (double)edge.min_y_x);
            scanline_edges.push_back(edge);
        }
        std::sort(scanline_edges.begin(), scanline_edges.end(), Scanline_Edge_Comparison);
        double z_delta = -(a / c);
        int current_y = static_cast<int>(lowest_y);
        int end_y = static_cast<int>(highest_y);
        while (current_y < end_y)
        {
            Scanline_Edge *left_edge, *right_edge;
            bool erased = true;
            while (erased)
            {
                erased = false;
                for (size_t j = 0; j < scanline_edges.size(); j++)
                {
                    if (scanline_edges[j].max_y <= current_y)
                    {
                        scanline_edges.erase(scanline_edges.begin() + j);
                        erased = true;
                        break;
                    }
                }
            }
            left_edge = &scanline_edges[0];
            right_edge = &scanline_edges[1];
            double start_z;
            int start_x, end_x;
            if (left_edge->min_y_x > right_edge->min_y_x)
            {
                start_x = static_cast<int>(right_edge->min_y_x);
                end_x = static_cast<int>(left_edge->min_y_x);
                start_z = right_edge->min_y_z;
            }
            else
            {
                start_x = static_cast<int>(left_edge->min_y_x);
                end_x = static_cast<int>(right_edge->min_y_x);
                start_z = left_edge->min_y_z;
            }
            for (size_t x = start_x; x <= end_x; x++)
            {
                if (z_buffer(current_y, x) > start_z)
                {
                    z_buffer(current_y, x) = start_z;
                    color_buffer(current_y, x) = 0xFF00AAAA;
                }
                start_z += z_delta;
            }
            left_edge->min_y_x += left_edge->x_y_delta;
            right_edge->min_y_x += right_edge->x_y_delta;
            left_edge->min_y_z += left_edge->z_y_delta;
            right_edge->min_y_z += right_edge->z_y_delta;
            current_y++;
        }
    }
}

void Scene::Pipeline::apply_z_buffer(std::vector<Poly::Polyhedron> &polyhedra, SDL_Renderer *renderer, SDL_Window *window)
{
    if (this->pipeline_altered)
    {
        z_buffer = Eigen::MatrixXd(screen.h, screen.w);
        z_buffer.setConstant(std::numeric_limits<double>::infinity());
        color_buffer = Eigen::MatrixXi(screen.h, screen.w);
        color_buffer.setConstant(0x00000000);
        for (int i = 0; i < polyhedra.size(); i++)
            apply_z_buffer_to_polyhedron(polyhedra[i], z_buffer, color_buffer);
        Eigen::Matrix<Uint32, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> color_matrix = color_buffer.cast<Uint32>();
        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(color_matrix.data(), color_matrix.cols(), color_matrix.rows(), 32, color_matrix.cols() * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        if (this->z_buffer_cache != NULL)
            SDL_DestroyTexture(this->z_buffer_cache);
        this->z_buffer_cache = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        
    }
    SDL_Rect dst_rect = this->screen;
    dst_rect.y -= this->screen.y;
    SDL_RenderCopy(renderer, this->z_buffer_cache, NULL, &dst_rect);
}

// Pipeline Main Flux

void Scene::Pipeline::render(SDL_Renderer *renderer, SDL_Window *window)
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
            {
                polyhedra[i].vertices[j](0) /= h_factors[i][j];
                polyhedra[i].vertices[j](1) /= h_factors[i][j];
            };
        }
    }
    if (use_z_buffer)
        apply_z_buffer(polyhedra, renderer, window);
    apply_shading(polyhedra, renderer);
    this->pipeline_altered = false;
}

// Wireframe

Poly::Polyhedron Scene::create_wireframe(std::vector<SDL_FPoint> generatrix, int slices)
{
    return wireframe(generatrix, slices);
}