#include <eigen3/Eigen/Core>
#include <iostream>
#include "pipeline.hpp"

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

    Poly::Polyhedron slice(segments, vectors);

    // Translate to origin & Mirror
    Eigen::Vector3d center = slice.get_center();
    slice.translate(-center);
    slice.mirror(false, true, false);
    slice.translate(0, std::abs(center(1)), 0);

    // Generate
    Poly::Polyhedron result = slice;

    std::vector<size_t> unshared_vertices;
    for (size_t i = 0; i < slice.vertices.size(); i++)
    {
        if (slice.vertices[i].y() != 0)
            unshared_vertices.push_back(i);
    }

    Poly::Polyhedron previous_slice = slice;

    float degrees = 360 / (float)slices;
    float radians = degrees * (M_PI / 180);

    for (int i = 0; i < slices - 1; i++)
    {
        slice.rotate(radians, 0, 0);
        for (int j = 0; j < unshared_vertices.size(); j++)
        {
            result.vertices.push_back(slice.vertices[unshared_vertices[j]]);
            Poly::Segment connection;
            connection.p1 = unshared_vertices[j - (i + 1) * unshared_vertices.size()];
            connection.p2 = unshared_vertices[j];
            result.segments.push_back(connection);
        }
        previous_slice = slice;
    }
    for (int i = 0; i < unshared_vertices.size(); i++)
    {
        
    }
    return result;

    std::vector<Poly::Face> faces;
    std::vector<size_t> visited_segments{0};
    std::vector<std::vector<size_t>> paths{{0}};
    // Breadth-First Search for Identifying the
    // While hasn't visited all segments
    while (visited_segments.size() != result.segments.size())
    {
        // For Each Path
        printf("Amount visited: %i\n", visited_segments.size());
        for (int i = 0; i < paths.size(); i++)
        {
            // printf("Searching Paths\n");
            // For Each Segment
            for (int j = 0; j < result.segments.size(); j++)
            {
                // printf("Searching Segments\n");
                Poly::Segment last_segment = result.segments[paths[i][paths[i].size() - 1]];
                // Check if last path segment is connected to this segment
                if (last_segment.p2 == result.segments[j].p1 || last_segment.p2 == result.segments[j].p2 || last_segment.p1 == result.segments[j].p1 || last_segment.p1 == result.segments[j].p2)
                {
                    // Checks if this segment has been visited already
                    bool already_visited = false;
                    for (int k = 0; k < visited_segments.size(); k++)
                    {
                        if (j == visited_segments[k])
                        {
                            already_visited = true;
                            break;
                        }
                    }
                    if (!already_visited)
                    {
                        std::vector<size_t> new_path = paths[i];
                        visited_segments.push_back(j);
                        new_path.push_back(j);
                        paths.push_back(new_path);
                        // Checks if the path is circular
                        if (result.segments[new_path[new_path.size() - 1]].p2 == result.segments[new_path[0]].p1 || result.segments[new_path[new_path.size() - 1]].p2 == result.segments[new_path[0]].p2 || result.segments[new_path[new_path.size() - 1]].p1 == result.segments[new_path[0]].p1 || result.segments[new_path[new_path.size() - 1]].p1 == result.segments[new_path[0]].p2)
                        {
                            Poly::Face face;
                            face.segments = new_path;
                            bool has_face_already = false;
                            for (int l = 0; l < result.faces.size(); l++)
                            {
                                if (face == result.faces[l])
                                    has_face_already = true;
                            }
                            if (!has_face_already)
                                result.faces.push_back(face);
                        }
                    }
                }
            }
        }
    }

    return result;
}