#ifndef WIREFRAME_H
#define WIREFRAME_H

#include <SDL2/SDL.h>
#include <vector>
#include "../../eigen-3.4.0/Eigen/Core"
#include "geometry.hh"
#include "gobject.hh"

using Eigen::Vector2d;
using Eigen::Vector3d;

class Wireframe
{
private:
    std::vector<Vector2d> ordered_points;

public:
    Wireframe(){};
    Wireframe(std::vector<SDL_FPoint> ordered_points)
    {
        for (int i = 0; i < ordered_points.size(); i++)
            this->ordered_points.push_back({ordered_points[i].x, ordered_points[i].y});
    }

    GObject3d revolution(int n = 3)
    {
        if (n <= 0)
        {
            printf("Null or Negative Slices Revolution Error. Quitting.\n");
            std::exit(1);
        }
        std::vector<Segment3d> segments;
        std::vector<Vector3d> points3d;
        for (int i = 0; i < this->ordered_points.size(); i++)
            points3d.push_back(Vector3d(ordered_points[i].x(), ordered_points[i].y(), 0));
        for (int i = 1; i < points3d.size(); i++)
            segments.push_back({points3d[i - 1], points3d[i]});
        GObject3d slice = GObject3d(segments, points3d);
        // Translate to center and mirror
        Vector3d center = slice.get_center();
        slice.translate(-center);
        slice.mirror(false, true, false);
        // Translate min y to 0
        double min_y = slice.vertices[0].y();
        for (int i = 1; i < slice.vertices.size(); i++)
        {
            if (slice.vertices[i].y() < min_y)
                min_y = slice.vertices[i].y();
        }
        printf("Minimun y: %f\n", min_y);
        slice.translate(0, -min_y, 0);
        // Create result object to be constructed upon
        GObject3d result = slice;
        // Create slices and rotate accordingly
        std::vector<int> non_mirrored_vertices;
        for (int i = 0; i < slice.vertices.size(); i++)
        {
            if (slice.vertices[i].y() != 0)
                non_mirrored_vertices.push_back(i);
        }
        // Rotate while connecting vertices
        GObject3d previous_slice = slice;
        float degrees = 360 / (float)n;
        float radians = degrees * (M_PI / 180);
        for (int i = 0; i < n - 1; i++)
        {
            printf("Current angle: %f\n", (i + 1) * degrees);
            slice.rotate(radians, 0, 0);
            for (int j = 0; j < non_mirrored_vertices.size(); j++)
            {
                result.vertices.push_back(slice.vertices[non_mirrored_vertices[j]]);
                Segment3d connection;
                connection.p1 = previous_slice.vertices[non_mirrored_vertices[j]];
                connection.p2 = slice.vertices[non_mirrored_vertices[j]];
                result.segments.push_back(connection);
            }
            previous_slice = slice;
        }
        return result;
    }
};

#endif