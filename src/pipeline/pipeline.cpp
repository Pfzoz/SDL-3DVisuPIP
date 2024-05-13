#include <iostream>
#include "pipeline.hpp"
#include <map>

Pip::Pipeline::Pipeline() {}

Pip::Pipeline &Pip::Pipeline::get_pipeline()
{
    static Pipeline instance;
    return instance;
}

Poly::Polyhedron Pip::wireframe(std::vector<SDL_FPoint> generatrix_points, int slices)
{
    if (slices <= 0)
    {
        std::cout << "Null or Negative Slices Error. Quitting." << std::endl;
        std::exit(1);
    }

    std::vector<Poly::Segment> segments;
    std::vector<Eigen::Vector3d> vectors;

    for (SDL_FPoint point : generatrix_points)
        vectors.push_back(Eigen::Vector3d(point.x, point.y, 0));
    for (size_t i = 1; i < vectors.size(); i++)
        segments.push_back(Poly::Segment{i - 1, i});

    Poly::Polyhedron slice(segments, vectors, {});

    // Generate
    Poly::Polyhedron result = slice;

    std::vector<size_t> unshared_vertices, shared_vertices;
    for (size_t i = 0; i < slice.vertices.size(); i++)
    {
        if (slice.vertices[i].y() != 0)
            unshared_vertices.push_back(i);
        else
            shared_vertices.push_back(i);
    }

    Poly::Polyhedron previous_slice = slice;

    float degrees = 360 / (float)slices;
    float radians = degrees * (M_PI / 180);

    std::map<size_t, size_t> unshared_map;
    for (int i = 0; i < unshared_vertices.size(); i++)
        unshared_map[unshared_vertices[i]] = i;
    printf("Unshared vertices: %i\n", unshared_vertices.size());
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
                result.segments.push_back(segment_copy);
            }
        }
        // Connect the new slice with previous slice
        for (int j = 0; j < unshared_vertices.size(); j++)
        {
            result.vertices.push_back(slice.vertices[unshared_vertices[j]]);
            Poly::Segment connection;
            connection.p2 = new_vertices_pos + j;
            if (i == 0)
                connection.p1 = unshared_vertices[j];
            else
                connection.p1 = previous_vertices_pos + j;
            result.segments.push_back(connection);
        }
        previous_slice = slice;
    }
    // Makes the connection of the final slice with the first one
    if (slices > 2)
    {
        for (int i = 0; i < unshared_vertices.size(); i++)
        {
            Poly::Segment connection;
            connection.p2 = result.vertices.size() - 1 - i;
            connection.p1 = unshared_vertices[unshared_vertices.size() - 1 - i];
            result.segments.push_back(connection);
        }
    }

    bool reached_final_vertex = false;
    size_t current_vertex = 0;
    // Identify faces
    while (!reached_final_vertex)
    {
        Poly::Face face;
        bool is_shared = false;
        for (int i = 0; i < shared_vertices.size(); i++)
        {
            if (shared_vertices[i] == current_vertex)
            {
                is_shared = true;
                break;
            }
        }
        for (int i = 0; i < result.segments.size(); i++)
        {
            if (result.segments[i].p1 == current_vertex && result.segments[i].p2 == current_vertex + slice.vertices.size())
            {
                face.segments.push_back(i);
                current_vertex += slice.vertices.size();
                break;
            }
        }
        bool next_shared = false;
        for (int i = 0; i < result.segments.size(); i++)
        {
            if (result.segments[i].p1 == current_vertex && result.segments[i].p2 == current_vertex - 2)
            {
                face.segments.push_back(i);
                current_vertex -= 2;
                next_shared = true;
                break;
            }
        }
        if (!next_shared)
        {
            for (int i = 0; i < result.segments.size(); i++)
            {
                if (result.segments[i].p1 == current_vertex && result.segments[i].p2 == current_vertex - 1)
                {
                    face.segments.push_back(i);
                    current_vertex -= 1;
                    break;
                }
            }
            for (int i = 0; i < result.segments.size(); i++)
            {
                if (result.segments[i].p1 == current_vertex && result.segments[i].p2 == current_vertex - 3)
                {
                    face.segments.push_back(i);
                    current_vertex -= 3;
                    break;
                }
            }
        }
        else
        {
            for (int i = 0; i < result.segments.size(); i++)
            {
                if (result.segments[i].p1 == current_vertex && result.segments[i].p2 == current_vertex + 1)
                {
                    face.segments.push_back(i);
                    current_vertex += 1;
                    break;
                }
            }
        }
        if (current_vertex == slice.vertices.size())
            result.faces.push_back(face);
    }

    return result;
}