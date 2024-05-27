#include "scene.hpp"

const double epsilon = 0.0001;

struct Scanline_Edge
{
    int min_y, max_y;
    int min_y_x, max_y_x;
    double min_y_z, max_y_z;
    double x_y_delta, z_y_delta;
};

struct Active_Edge
{
    float min_y_x, min_y_z, y_max, y_min;
    float m_inversed, m_z_inversed;
};

Scene::Pipeline::Pipeline() {}

Scene::Pipeline &Scene::Pipeline::get_pipeline()
{
    static Pipeline instance;
    return instance;
}

// Setters
void Scene::Pipeline::translate_camera(double x, double y, double z)
{
    this->camera.translate(x, y, z);
    this->pipeline_altered = true;
}

void Scene::Pipeline::rotate_camera(double x, double y, double z)
{
    this->camera.rotate(x, y, z);
    this->pipeline_altered = true;
}

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

void Scene::Pipeline::set_projection_distance(double distance)
{
    this->projection_distance = distance;
    this->pipeline_altered = true;
}

void Scene::Pipeline::use_projection(Projection projection_mode)
{
    this->projection = projection_mode;
    this->pipeline_altered = true;
}

void Scene::Pipeline::use_zbuffer(bool use_z_buffer)
{
    this->use_z_buffer = use_z_buffer;
    this->pipeline_altered = true;
}

void Scene::Pipeline::use_shading(Shading shading_mode)
{
    this->shading = shading_mode;
    this->pipeline_altered = true;
}

// Getters
bool Scene::Pipeline::using_z_buffer()
{
    return this->use_z_buffer;
}

bool Scene::Pipeline::using_painter_clipper()
{
    return this->use_painter_clipper;
}

void Scene::Pipeline::use_painter_clip(bool use_painter_clipper)
{
    this->use_painter_clipper = use_painter_clipper;
    this->pipeline_altered = true;
}

Scene::Shading Scene::Pipeline::get_shading()
{
    return this->shading;
}

void Scene::Pipeline::get_vrp(double *x, double *y, double *z)
{
    Eigen::Vector3d vrp = this->camera.get_vrp();
    *x = vrp(0);
    *y = vrp(1);
    *z = vrp(2);
}

double Scene::Pipeline::get_projection_distance()
{
    return this->projection_distance;
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
    // double zvp = (camera.get_focal_point() - camera.get_vrp()).norm();
    double dp_distance = projection_distance;
    matrix << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, ((-dp_distance) / dp_distance), dp_distance * ((zprp / dp_distance)),
        0, 0, -(1 / dp_distance), (zprp / dp_distance);
    return matrix;
}

// Shading
void Scene::Pipeline::apply_shading(std::vector<Poly::Polyhedron> polyhedra, SDL_Renderer *renderer)
{
    switch (shading)
    {
    case Shading::NO_SHADING:
        if (!this->use_z_buffer)
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
        for (int j = 0; j < polyhedra[i].faces.size(); j++)
        {
            for (int k = 0; k < polyhedra[i].faces[j].segments.size(); k++)
            {
                Poly::Segment seg = polyhedra[i].segments[polyhedra[i].faces[j].segments[k]];
                Eigen::Vector3d v1 = polyhedra[i].vertices[seg.p1];
                Eigen::Vector3d v2 = polyhedra[i].vertices[seg.p2];
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawLine(renderer, (int)(v1.x()), (int)(v1.y()),
                                   (int)(v2.x()), (int)(v2.y()));
            }
        }
    }
}

// Objects
void Scene::Pipeline::get_object_center(int index, double *x, double *y, double *z)
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

void Scene::Pipeline::remove_object(int index)
{
    scene_objects.erase(scene_objects.begin() + index);
    this->pipeline_altered = true;
}

void Scene::Pipeline::translate_object(int index, double x, double y, double z)
{
    scene_objects[index].translate(x, y, z);
    this->pipeline_altered = true;
}

void Scene::Pipeline::rotate_object(int index, double x, double y, double z)
{
    scene_objects[index].rotate(x, y, z);
    this->pipeline_altered = true;
}

void Scene::Pipeline::scale_object(int index, double x, double y, double z)
{
    scene_objects[index].scale(x, y, z);
    this->pipeline_altered = true;
}

uint Scene::Pipeline::get_object_color(int index)
{
    return scene_objects[index].get_color();
}

void Scene::Pipeline::get_object_ambient_coefficients(int index, double *r, double *g, double *b)
{
    scene_objects[index].get_ambient_reflection_coefficients(r, g, b);
}

void Scene::Pipeline::get_object_specular_coefficients(int index, double *r, double *g, double *b)
{
    scene_objects[index].get_specular_reflection_coefficients(r, g, b);
}

void Scene::Pipeline::get_object_diffuse_coefficients(int index, double *r, double *g, double *b)
{
    scene_objects[index].get_diffuse_reflection_coefficients(r, g, b);
}

void Scene::Pipeline::set_object_ambient_coefficients(int index, double r, double g, double b)
{
    scene_objects[index].set_ambient_reflection_coefficients(r, g, b);
    this->pipeline_altered = true;
}

void Scene::Pipeline::set_object_specular_coefficients(int index, double r, double g, double b)
{
    scene_objects[index].set_specular_reflection_coefficients(r, g, b);
    this->pipeline_altered = true;
}

void Scene::Pipeline::set_object_diffuse_coefficients(int index, double r, double g, double b)
{
    scene_objects[index].set_diffuse_reflection_coefficients(r, g, b);
    this->pipeline_altered = true;
}

void Scene::Pipeline::apply_painter_clipper(std::vector<Poly::Polyhedron> &polyhedra)
{
    for (int i = 0; i < polyhedra.size(); i++)
    {
        int erased_count = 0;
        for (int j = (int)polyhedra[i].faces.size() - 1; j > -1; j--)
        {
            Poly::Segment v_seg = polyhedra[i].segments[polyhedra[i].faces[j].segments[0]];
            Poly::Segment u_seg = polyhedra[i].segments[polyhedra[i].faces[j].segments[1]];
            Eigen::Vector3d p1, p2, p3, p4;
            p1 = polyhedra[i].vertices[v_seg.p1];
            p2 = polyhedra[i].vertices[v_seg.p2];
            p3 = polyhedra[i].vertices[u_seg.p1];
            p4 = polyhedra[i].vertices[u_seg.p2];
            Eigen::Vector3d v, u, n;
            v = p1 - p2;
            u = p3 - p4;
            if (v_seg.successor == 1 && u_seg.successor == 1)
                n = u.cross(v);
            else
                n = v.cross(u);
            n.normalize();
            Eigen::Vector3d vrp = camera.get_vrp();
            double a = n(0), b = n(1), c = n(2);
            double d = -(a * p2.x()) - (b * p2.y()) - (c * p2.z());
            Eigen::Vector3d center = p2 + a * (p1 - p2) + b * (p3 - p2);
            Eigen::Vector3d o = vrp - center;
            o.normalize();
            if (o.dot(n) <= 0)
            {
                erased_count++;
                polyhedra[i].faces.erase(polyhedra[i].faces.begin() + j);
            }
        }
    }
}

// Lights

void Scene::Pipeline::set_lights_intensity(double r, double g, double b)
{
    this->illumination_intensity_b = b;
    this->illumination_intensity_g = g;
    this->illumination_intensity_r = r;
    this->pipeline_altered = true;
}

void Scene::Pipeline::get_illumination_intensity(double *r, double *g, double *b)
{
    *r = this->illumination_intensity_r;
    *g = this->illumination_intensity_g;
    *b = this->illumination_intensity_b;
}

uint Scene::Pipeline::apply_lights(Poly::Polyhedron &poly, int face_index)
{
    Poly::Segment v_seg = poly.segments[poly.faces[face_index].segments[0]];
    Poly::Segment u_seg = poly.segments[poly.faces[face_index].segments[1]];
    Eigen::Vector3d p1, p2, p3, p4;
    p1 = poly.vertices[v_seg.p1];
    p2 = poly.vertices[v_seg.p2];
    p3 = poly.vertices[u_seg.p1];
    p4 = poly.vertices[u_seg.p2];
    Eigen::Vector3d v, u, n;
    v = p1 - p2;
    u = p3 - p4;
    if (v_seg.successor == 1 && u_seg.successor == 1)
        n = u.cross(v);
    else
        n = v.cross(u);
    n.normalize();
    double a = n(0), b = n(1), c = n(2);
    double d = -(a * p2.x()) - (b * p2.y()) - (c * p2.z());
    Eigen::Vector3d center = p2 + a * (p1 - p2) + b * (p3 - p2);
    Eigen::Vector3d s = this->camera.get_vrp() - center;
    Eigen::Vector3d light_vector = lights_position - center;
    light_vector.normalize();
    Eigen::Vector3d r = (2 * light_vector.dot(n)) * n - light_vector;
    r.normalize();
    double specular_angle = r.dot(s);
    specular_angle = pow(specular_angle, poly.get_specular_exponent());
    double angle = n.dot(light_vector);
    if (angle < 0)
        angle = 0;
    if (angle > 1)
        angle = 1;
    if (specular_angle < 0)
        specular_angle = 0;
    if (specular_angle > 1)
        specular_angle = 1;
    double light_distance = (lights_position - center).norm();
    double light_attenuation = std::min(1.0, 1.0 / (light_distance * light_distance));
    double ac_r, ac_g, ac_b;
    double dc_r, dc_g, dc_b;
    double sc_r, sc_g, sc_b;
    poly.get_ambient_reflection_coefficients(&ac_r, &ac_g, &ac_b);
    poly.get_diffuse_reflection_coefficients(&dc_r, &dc_g, &dc_b);
    poly.get_specular_reflection_coefficients(&sc_r, &sc_g, &sc_b);
    double diffuse_illumination_r = angle * this->illumination_intensity_r * dc_r;
    double diffuse_illumination_g = angle * this->illumination_intensity_g * dc_g;
    double diffuse_illumination_b = angle * this->illumination_intensity_b * dc_b;
    double ambient_illumination_r = this->illumination_intensity_r * ac_r;
    double ambient_illumination_g = this->illumination_intensity_g * ac_g;
    double ambient_illumination_b = this->illumination_intensity_b * ac_b;
    double specular_illumination_r = this->ambient_light_intensity_r * sc_r * specular_angle;
    double specular_illumination_g = this->ambient_light_intensity_g * sc_g * specular_angle;
    double specular_illumination_b = this->ambient_light_intensity_b * sc_b * specular_angle;
    int total_illumination_r = std::clamp((int)(diffuse_illumination_r + light_attenuation * (ambient_illumination_r + specular_illumination_r)), 0, 255);
    int total_illumination_g = std::clamp((int)(diffuse_illumination_g + light_attenuation * (ambient_illumination_g + specular_illumination_g)), 0, 255);
    int total_illumination_b = std::clamp((int)(diffuse_illumination_b + light_attenuation * (ambient_illumination_b + specular_illumination_b)), 0, 255);
    uint final_color = 0xFF000000 | (total_illumination_b) << 16 | (total_illumination_g) << 8 | (total_illumination_r);
    return final_color;
}

void Scene::Pipeline::get_ambient_light_intensity(double *r, double *g, double *b)
{
    *r = this->ambient_light_intensity_r;
    *g = this->ambient_light_intensity_g;
    *b = this->ambient_light_intensity_b;
}

void Scene::Pipeline::set_ambient_light(double r, double g, double b)
{
    this->ambient_light_intensity_r = r;
    this->ambient_light_intensity_g = g;
    this->ambient_light_intensity_b = b;
    this->pipeline_altered = true;
}

double Scene::Pipeline::get_object_specular_exponent(int index)
{
    return this->scene_objects[index].get_specular_exponent();
}

void Scene::Pipeline::set_object_specular_exponent(int index, double specular_exponent)
{
    this->scene_objects[index].set_specular_exponent(specular_exponent);
    this->pipeline_altered = true;
}

void Scene::Pipeline::get_lights_position(double *x, double *y, double *z)
{
    *x = this->lights_position.x();
    *y = this->lights_position.y();
    *z = this->lights_position.z();
}

void Scene::Pipeline::set_lights_position(double x, double y, double z)
{
    this->lights_position = Eigen::Vector3d(x, y, z);
    this->pipeline_altered = true;
}

// Z-Buffer

bool Scanline_Edge_ComparisonY(Scanline_Edge a, Scanline_Edge b)
{
    return a.min_y < b.min_y;
}

bool Active_Edge_Comparison(Active_Edge a, Active_Edge b)
{
    return a.min_y_x < b.min_y_x;
}

void Scene::Pipeline::apply_z_buffer_to_polyhedron(Poly::Polyhedron poly, Eigen::MatrixXd &z_buffer, Eigen::MatrixXi &color_buffer)
{
    for (int i = 0; i < poly.faces.size(); i++)
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
        double z_delta = -(a / c);
        double lowest_y = std::numeric_limits<double>::max();
        double highest_y = std::numeric_limits<double>::lowest();
        for (int seg_j : poly.faces[i].segments)
        {
            Scanline_Edge edge;
            Eigen::Vector3d p1, p2;
            p1 = poly.vertices[poly.segments[seg_j].p1];
            p2 = poly.vertices[poly.segments[seg_j].p2];
            if (p2.y() == p1.y())
            {
                int highest_x = (int)p2.x();
                int lowest_x = (int)p1.x();
                int const_y = (int)p1.y();
                double start_z = p1.z();
                if (p1.x() > highest_x)
                {
                    std::swap(highest_x, lowest_x);
                    start_z = p2.z();
                }
                for (int j = lowest_x; j <= highest_x; j++)
                {
                    if (z_buffer(const_y, j) > start_z)
                    {
                        z_buffer(const_y, j) = start_z;
                        color_buffer(const_y, j) = poly.get_color();
                    }
                    start_z += z_delta;
                }
                continue;
            }
            if (p1.y() < p2.y())
                std::swap(p1, p2);
            edge.max_y = (int)p1.y();
            edge.max_y_x = (int)p1.x();
            edge.max_y_z = p1.z();
            edge.min_y = (int)p2.y();
            edge.min_y_x = (int)p2.x();
            edge.min_y_z = p2.z();
            if (edge.min_y < lowest_y)
                lowest_y = edge.min_y;
            if (edge.max_y > highest_y)
                highest_y = edge.max_y;
            edge.z_y_delta = ((double)edge.max_y_z - (double)edge.min_y_z) / ((double)edge.max_y - (double)edge.min_y);
            edge.x_y_delta = ((double)edge.max_y_x - (double)edge.min_y_x) / ((double)edge.max_y - (double)edge.min_y);
            scanline_edges.push_back(edge);
        }
        std::sort(scanline_edges.begin(), scanline_edges.end(), Scanline_Edge_ComparisonY);
        int current_y = (int)(lowest_y);
        int end_y = (int)(highest_y);
        std::vector<Active_Edge> active_edges;
        while (current_y <= end_y && current_y < z_buffer.rows())
        {
            for (int j = (int)(scanline_edges.size()) - 1; j > -1; j--)
            {
                if (scanline_edges[j].min_y == current_y)
                {
                    Active_Edge active_edge{(float)scanline_edges[j].min_y_x,
                                            (float)scanline_edges[j].min_y_z,
                                            (float)scanline_edges[j].max_y,
                                            (float)scanline_edges[j].min_y,
                                            (float)scanline_edges[j].x_y_delta,
                                            (float)scanline_edges[j].z_y_delta};
                    active_edges.push_back(active_edge);
                    scanline_edges.erase(scanline_edges.begin() + j);
                }
            }
            for (int j = active_edges.size() - 1; j > -1; j--)
            {
                if (active_edges[j].y_max == current_y)
                    active_edges.erase(active_edges.begin() + j);
            }
            std::sort(active_edges.begin(), active_edges.end(), Active_Edge_Comparison);
            if (current_y < 0)
            {
                current_y++;
                continue;
            }
            for (int j = 0; j < (int)(active_edges.size()) - 1; j += 2)
            {
                int start_x = active_edges[j].min_y_x;
                int end_x = active_edges[j + 1].min_y_x;
                double start_z = active_edges[j].min_y_z;
                double end_z = active_edges[j + 1].min_y_z;
                uint color = this->apply_lights(poly, i);
                if (current_y == lowest_y || current_y == highest_y)
                    color = 0xFFFFFFFF;
                if (start_x >= 0 && start_x < z_buffer.cols() && z_buffer(current_y, start_x) > start_z)
                {
                    z_buffer(current_y, start_x) = start_z;
                    color_buffer(current_y, start_x) = 0xFFFFFFFF;
                }
                if (end_x >= 0 && end_x < z_buffer.cols() && z_buffer(current_y, end_x) > end_z)
                {
                    z_buffer(current_y, end_x) = end_z;
                    color_buffer(current_y, end_x) = 0xFFFFFFFF;
                }
                for (int x = start_x + 1; x < end_x - 1 && x < z_buffer.cols(); x++)
                {
                    if (x < 0)
                        continue;
                    if (z_buffer(current_y, x) > start_z)
                    {
                        z_buffer(current_y, x) = start_z;
                        color_buffer(current_y, x) = color;
                    }
                    start_z += z_delta;
                }
                active_edges[j].min_y_x += active_edges[j].m_inversed;
                active_edges[j + 1].min_y_x += active_edges[j + 1].m_inversed;
                active_edges[j].min_y_z += active_edges[j].m_z_inversed;
                active_edges[j + 1].min_y_z += active_edges[j + 1].m_z_inversed;
            }
            current_y++;
        }
    }
}

void Scene::Pipeline::apply_z_buffer(std::vector<Poly::Polyhedron> &polyhedra, SDL_Renderer *renderer, SDL_Window *window)
{
    if (this->pipeline_altered)
    {
        printf("Pipeline Altered\n");
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
    if (use_painter_clipper)
        apply_painter_clipper(polyhedra);
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
    else if (projection == Projection::PARALLEL)
    {
        for (int i = 0; i < polyhedra.size(); i++)
        {
            for (int j = 0; j < polyhedra[i].vertices.size(); j++)
                polyhedra[i].vertices[j](2) = this->scene_objects[i].vertices[j](0);
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