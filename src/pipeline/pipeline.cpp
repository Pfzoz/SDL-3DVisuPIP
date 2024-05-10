#include <eigen3/Eigen/Core>
#include <iostream>
#include "pipeline.hpp"

Poly::Polyhedron Pipeline::wireframe(std::vector<SDL_FPoint> generatrix_points, int slices)
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
    for (int i = 1; i < vectors.size(); i++)
        segments.push_back(Poly::Segment{&vectors[i - 1], &vectors[i]});

    Poly::Polyhedron slice(segments, vectors);

    // Translate to origin & Mirror
    Eigen::Vector3d center = slice.get_center();
    slice.translate(-center);
    slice.mirror(false, true, false);
    slice.translate(0, std::abs(center(1)), 0);

    // Generate
    Poly::Polyhedron result(slice);

    std::vector<int> unshared_vertices;
    for (int i = 0; i < slice.vertices.size(); i++)
    {
        if (slice.vertices[i].y() != 0)
            unshared_vertices.push_back(i);
    }

    Poly::Polyhedron previous_slice(slice);

    float degrees = 360 / (float)slices;
    float radians = degrees * (M_PI / 180);

    for (int i = 0; i < slices - 1; i++)
    {
        slice.rotate(radians, 0, 0);
        for (int j = 0; j < unshared_vertices.size(); j++)
        {
            result.vertices.push_back(slice.vertices[unshared_vertices[j]]);
            Poly::Segment connection;
            connection.p1 = &result.vertices[unshared_vertices[j - (i + 1) * unshared_vertices.size()]];
            connection.p2 = &result.vertices[unshared_vertices[j]];
            result.segments.push_back(connection);
        }
        previous_slice = Poly::Polyhedron(slice);
    }

    return result;
}