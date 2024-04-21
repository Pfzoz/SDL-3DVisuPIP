#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "../../eigen-3.4.0/Eigen/Core"

using Eigen::Matrix4d;

Matrix4d x_rotation_matrix3d(double angle)
{
    return Matrix4d(1, 0, 0, 0, 0, std::cos(angle), -std::sin(angle), 0, 0, std::sin(angle), std::cos(angle), 0, 0, 0, 0, 1);
}

Matrix4d y_rotation_matrix3d(double angle)
{
    return Matrix4d(std::cos(angle), 0, std::sin(angle), 0, 0, 1, 0, 0, -std::sin(angle), 0, std::cos(angle), 0, 0, 0, 0, 1);
}

Matrix4d z_rotation_matrix3d(double angle)
{
    return Matrix4d(std::cos(angle), -std::sin(angle), 0, 0, std::sin(angle), std::cos(angle), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

#endif