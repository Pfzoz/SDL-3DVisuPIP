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
        for (int i = 0; i < ordered_points.size() - 1; i++)
            ordered_points.push_back({ordered_points[i].x, ordered_points[i].y});
    }

    GObject3d revolution(int slices = 2)
    {
        if (slices <= 0)
        {
            printf("Null or Negative Slices Revolution Error. Quitting.\n");
            std::exit(1);
        }
        std::vector<Segment3d> segments;
        std::vector<Vector3d *> points3d;
        for (int i = 0; i < ordered_points.size() - 1; i++)
        {
            points3d.push_back(new Vector3d(ordered_points[i].x(), ordered_points[i].y(), 0));
        }
        for (int i = 1; i < points3d.size(); i++)
        {
            segments.push_back({points3d[i - 1], points3d[i]});
        }
        GObject3d slice = GObject3d(segments, points3d);
        
        return GObject3d();
    }
};

#endif