#include <iostream>
#include "pipeline.hpp"
#include <map>

Pip::Pipeline::Pipeline() {}

Pip::Pipeline &Pip::Pipeline::get_pipeline()
{
    static Pipeline instance;
    return instance;
}

// Setters
void Pip::Pipeline::set_vrp(double x, double y, double z)
{
    this->camera.set_vrp({x, y, z});
}

void Pip::Pipeline::set_focal_point(double x, double y, double z)
{
    this->camera.set_focal_point({x, y, z});
}

// Getters
void Pip::Pipeline::get_vrp(double *x, double *y, double *z)
{
    Eigen::Vector3d vrp = this->camera.get_vrp();
    *x = vrp(0);
    *y = vrp(1);
    *z = vrp(2);
}

void Pip::Pipeline::get_focal_point(double *x, double *y, double *z)
{
    Eigen::Vector3d focal_point = this->camera.get_focal_point();
    *x = focal_point(0);
    *y = focal_point(1);
    *z = focal_point(2);
}

void Pip::Pipeline::get_camera_view_direction(double *x, double *y, double *z)
{
    Eigen::Vector3d n = this->camera.get_n();
    *x = n(0);
    *y = n(1);
    *z = n(2);
}

void Pip::Pipeline::get_camera_view_right(double *x, double *y, double *z)
{
    Eigen::Vector3d u = this->camera.get_u();
    *x = u(0);
    *y = u(1);
    *z = u(2);
}

void Pip::Pipeline::get_camera_view_up(double *x, double *y, double *z)
{
    Eigen::Vector3d v = this->camera.get_v();
    *x = v(0);
    *y = v(1);
    *z = v(2);
}

// Wireframe
void rotate_slices(Poly::Polyhedron &poly, Poly::Polyhedron slice, int slices, std::vector<size_t> unshared_vertices)
{
    std::map<size_t, size_t> unshared_map;
    for (int i = 0; i < unshared_vertices.size(); i++)
        unshared_map[unshared_vertices[i]] = i;

    Poly::Polyhedron previous_slice = slice;

    float degrees = 360 / (float)slices;
    float radians = degrees * (M_PI / 180);
    // Make rotations and connections
    for (int i = 0; i < slices - 1; i++)
    {
        slice.rotate(radians, 0, 0);
        int new_vertices_pos = slice.vertices.size() + i * unshared_vertices.size();
        int previous_vertices_pos = slice.vertices.size() + (i - 1) * unshared_vertices.size();
        // Connct the internal segments for the new slices
        for (int j = 0; j < slice.segments.size(); j++)
        {
            bool is_p1_shared = true;
            bool is_p2_shared = true;
            for (int k = 0; k < unshared_vertices.size(); k++)
            {
                if (slice.segments[j].p1 == unshared_vertices[k])
                    is_p1_shared = false;
                if (slice.segments[j].p2 == unshared_vertices[k])
                    is_p2_shared = false;
                if (!is_p1_shared && !is_p2_shared)
                    break;
            }
            if (!is_p1_shared || !is_p2_shared)
            {
                Poly::Segment segment_copy = slice.segments[j];
                if (!is_p1_shared)
                    segment_copy.p1 = new_vertices_pos + (unshared_map[segment_copy.p1] % unshared_vertices.size());
                if (!is_p2_shared)
                    segment_copy.p2 = new_vertices_pos + (unshared_map[segment_copy.p2] % unshared_vertices.size());
                poly.segments.push_back(segment_copy);
            }
        }
        // Connect the new slice with previous slice
        for (int j = 0; j < unshared_vertices.size(); j++)
        {
            poly.vertices.push_back(slice.vertices[unshared_vertices[j]]);
            Poly::Segment connection;
            connection.p2 = new_vertices_pos + j;
            if (i == 0)
                connection.p1 = unshared_vertices[j];
            else
                connection.p1 = previous_vertices_pos + j;
            poly.segments.push_back(connection);
        }
        previous_slice = slice;
    }
    // Makes the connection of the final slice with the first one
    if (slices > 2)
    {
        for (int i = 0; i < unshared_vertices.size(); i++)
        {
            Poly::Segment connection;
            connection.p2 = poly.vertices.size() - 1 - i;
            connection.p1 = unshared_vertices[unshared_vertices.size() - 1 - i];
            poly.segments.push_back(connection);
        }
    }
}

// "Land Bridge" Algorithm
void identify_internal_faces(Poly::Polyhedron &poly, const Poly::Polyhedron &slice, int slices, std::vector<size_t> unshared_vertices, std::vector<size_t> shared_vertices,
                             std::map<size_t, std::vector<size_t>> &lands_map)
{
    size_t current_slice = 0;
    // Identify faces
    while (current_slice != slices)
    {
        std::vector<size_t> land_a = lands_map[current_slice];
        std::vector<size_t> land_b;
        if (current_slice != slices - 1)
            land_b = lands_map[current_slice + 1];
        else
            land_b = lands_map[0];
        size_t current_a = 0, current_b = 0, current_bridge = 0;
        size_t current_vertex = 0;
        while (current_vertex != slice.vertices.size() - 1)
        {
            Poly::Face face;
            bool is_shared = std::find(shared_vertices.begin(), shared_vertices.end(), current_vertex) != shared_vertices.end();
            bool bridge_travel = false, on_origin = false;
            // Shared Vertex, Go to Land B, add {Shared Vertex, Land B}
            if (is_shared)
            {
                current_bridge++;
                for (size_t i = 0; i < poly.segments.size(); i++)
                {
                    if ((poly.segments[i].p1 == current_vertex && poly.segments[i].p2 == land_b[current_b]) || (poly.segments[i].p1 == land_b[current_b] && poly.segments[i].p2 == current_vertex))
                    {
                        face.segments.push_back(i);
                        break;
                    }
                }
            }
            // Unshared Vertex, Go to Land B, add {Land A, Land B}
            else
            {
                for (size_t i = 0; i < poly.segments.size(); i++)
                {
                    if ((poly.segments[i].p1 == land_a[current_a] && poly.segments[i].p2 == land_b[current_b]) || (poly.segments[i].p1 == land_b[current_b] && poly.segments[i].p2 == land_a[current_a]))
                    {
                        face.segments.push_back(i);
                        current_a++;
                        current_b++;
                        break;
                    }
                }
                // Try Land B, else go to Bridge
                bool traveled = false;
                for (size_t i = 0; i < poly.segments.size(); i++)
                {
                    if ((poly.segments[i].p1 == land_b[current_b - 1] && poly.segments[i].p2 == land_b[current_b]) || (poly.segments[i].p1 == land_b[current_b] && poly.segments[i].p2 == land_b[current_b - 1]))
                    {
                        face.segments.push_back(i);
                        traveled = true;
                        break;
                    }
                }
                // Go To Bridge
                if (!traveled)
                {
                    for (size_t i = 0; i < poly.segments.size(); i++)
                    {
                        if ((poly.segments[i].p1 == land_b[current_b - 1] && poly.segments[i].p2 == shared_vertices[current_bridge]) || (poly.segments[i].p1 == shared_vertices[current_bridge] && poly.segments[i].p2 == land_b[current_b - 1]))
                        {
                            face.segments.push_back(i);
                            current_a--;
                            bridge_travel = true;
                            break;
                        }
                    }
                }
            }
            if (!bridge_travel)
            {
                // Go back to Land A, add {Land B, Land A}
                for (size_t i = 0; i < poly.segments.size(); i++)
                {
                    if ((poly.segments[i].p1 == land_a[current_a] && poly.segments[i].p2 == land_b[current_b]) || (poly.segments[i].p1 == land_b[current_b] && poly.segments[i].p2 == land_a[current_a]))
                    {
                        face.segments.push_back(i);
                        break;
                    }
                }
            }
            else
            {
                // Go back to origin, {Bridge, Origin}
                for (size_t i = 0; i < poly.segments.size(); i++)
                {
                    if ((poly.segments[i].p1 == land_a[current_a] && poly.segments[i].p2 == shared_vertices[current_bridge - 1]) || (poly.segments[i].p1 == shared_vertices[current_bridge - 1] && poly.segments[i].p2 == land_a[current_a]))
                    {
                        face.segments.push_back(i);
                        on_origin = true;
                        break;
                    }
                }
            }
            // Check if already on origin
            if (!on_origin)
            {
                // Close, Add {Land A, Origin}
                if (!is_shared) // Origin = Current a - 1
                {
                    for (size_t i = 0; i < poly.segments.size(); i++)
                    {
                        if ((poly.segments[i].p1 == land_a[current_a] && poly.segments[i].p2 == land_a[current_a - 1]) || (poly.segments[i].p1 == land_a[current_a - 1] && poly.segments[i].p2 == land_a[current_a]))
                        {
                            face.segments.push_back(i);
                            break;
                        }
                    }
                }
                else // Origin = current_vertex
                {
                    for (size_t i = 0; i < poly.segments.size(); i++)
                    {
                        if ((poly.segments[i].p1 == land_a[current_a] && poly.segments[i].p2 == current_vertex) || (poly.segments[i].p1 == current_vertex && poly.segments[i].p2 == land_a[current_a]))
                        {
                            face.segments.push_back(i);
                            break;
                        }
                    }
                }
            }
            if (bridge_travel)
                current_a++;
            current_vertex++;
            poly.faces.push_back(face);
        }
        current_slice++;
    }
}

void identify_external_faces(Poly::Polyhedron &poly, Poly::Polyhedron slice, int slices, std::vector<size_t> shared_vertices, std::map<size_t, std::vector<size_t>> &lands_map)
{
    // First Face
    bool is_shared = false;
    for (size_t i = 0; i < shared_vertices.size(); i++)
    {
        if (0 == shared_vertices[i])
        {
            is_shared = true;
            break;
        }
    }
    if (!is_shared)
    {
        Poly::Face face;
        for (size_t i = 0; i < slices; i++)
        {
            size_t next_i;
            if (i != slices - 1)
                next_i = i + 1;
            else
                next_i = 0;
            for (size_t j = 0; j < poly.segments.size(); j++)
            {
                if (poly.segments[j].p1 == lands_map[i][0] && poly.segments[j].p2 == lands_map[next_i][0] || poly.segments[j].p1 == lands_map[next_i][0] && poly.segments[j].p2 == lands_map[i][0])
                {
                    face.segments.push_back(j);
                    break;
                }
            }
        }
        poly.faces.push_back(face);
    }
    // Second Face
    is_shared = false;
    size_t last_vertex = slice.vertices.size() - 1;
    for (size_t i = 0; i < shared_vertices.size(); i++)
    {
        if (last_vertex == shared_vertices[i])
        {
            is_shared = true;
            break;
        }
    }
    if (!is_shared)
    {
        Poly::Face face;
        size_t last_land = lands_map[0].size() - 1;
        for (size_t i = 0; i < slices; i++)
        {
            size_t next_i;
            if (i != slices - 1)
                next_i = i + 1;
            else
                next_i = 0;
            for (size_t j = 0; j < poly.segments.size(); j++)
            {
                if (poly.segments[j].p1 == lands_map[i][last_land] && poly.segments[j].p2 == lands_map[next_i][last_land] || poly.segments[j].p1 == lands_map[next_i][last_land] && poly.segments[j].p2 == lands_map[i][last_land])
                {
                    face.segments.push_back(j);
                    break;
                }
            }
        }
        poly.faces.push_back(face);
    }
}

Poly::Polyhedron Pip::wireframe(std::vector<SDL_FPoint> generatrix_points, int slices)
{
    if (slices < 3)
    {
        std::cout << "Only 3 or more slices are supported." << std::endl;
        std::exit(1);
    }

    std::vector<Poly::Segment> segments;
    std::vector<Eigen::Vector3d> vectors;

    // Create Slice
    for (SDL_FPoint point : generatrix_points)
        vectors.push_back(Eigen::Vector3d(point.x, point.y, 0));
    for (size_t i = 1; i < vectors.size(); i++)
        segments.push_back(Poly::Segment{i - 1, i});

    Poly::Polyhedron slice(segments, vectors, {});

    // Generate
    Poly::Polyhedron result = slice;

    // Identify shared and unshared vertices
    std::vector<size_t> unshared_vertices, shared_vertices;
    for (size_t i = 0; i < slice.vertices.size(); i++)
    {
        if (slice.vertices[i].y() != 0)
            unshared_vertices.push_back(i);
        else
            shared_vertices.push_back(i);
    }

    // Rotate Slices and Connect them (Creates vertices and segments).
    rotate_slices(result, slice, slices, unshared_vertices);

    // Create "Lands" mapping for each slice to identify the faces
    // shared_vertices = bridges
    std::map<size_t, std::vector<size_t>> lands_map;
    // Use map to alternate between Land A and Land B
    std::vector<size_t> first_land_vertices;
    for (size_t i = 0; i < unshared_vertices.size(); i++)
        first_land_vertices.push_back(unshared_vertices[i]);
    lands_map[0] = first_land_vertices;
    for (int i = 1; i < slices; i++)
    {
        std::vector<size_t> vertices;
        size_t unshared_pos = 0;
        unshared_pos += slice.vertices.size();
        if (i > 1)
            unshared_pos += (i - 1) * unshared_vertices.size();
        for (int j = 0; j < unshared_vertices.size(); j++)
            vertices.push_back(unshared_pos + j);
        lands_map[i] = vertices;
    }

    identify_internal_faces(result, slice, slices, unshared_vertices, shared_vertices, lands_map);
    identify_external_faces(result, slice, slices, shared_vertices, lands_map);

    return result;
}