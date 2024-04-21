#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../../eigen-3.4.0/Eigen/Core"

using Eigen::Vector2d;
using Eigen::Vector3d;

struct Segment3d
{
    Vector3d *p1;
    Vector3d *p2;  
};

struct Segment2d
{
    Vector2d *p1;
    Vector2d *p2;
};

#endif