#ifndef GOBJECT_H
#define GOBJECT_H

#include <vector>
#include "../../eigen-3.4.0/Eigen/Core"
#include "geometry.hh"

using Eigen::MatrixXd;
using Eigen::Vector2d;
using Eigen::Vector3d;

class GObject3d
{
public:
    std::vector<Vector3d *> vertices;
    std::vector<Segment3d> segments;

    GObject3d(){};
    GObject3d(std::vector<Segment3d> segments, std::vector<Vector3d *> vertices)
    {
        this->segments = segments;
        this->vertices = vertices;
    }
};

class GObject2d
{
public:
    std::vector<Vector2d *> vertices;
    std::vector<Segment2d> segments;

    GObject2d(){};
    GObject2d(std::vector<Segment2d> segments, std::vector<Vector2d *> vertices)
    {
        this->segments = segments;
        this->vertices = vertices;
    }
};

#endif