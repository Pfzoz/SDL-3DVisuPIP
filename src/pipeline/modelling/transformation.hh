#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "../../eigen-3.4.0/Eigen/Core"

using Eigen::Matrix4d;

Matrix4d x_rotation_matrix3d(double angle)
{
    Matrix4d mat;
    mat << 1, 0, 0, 0, 0, std::cos(angle), -std::sin(angle), 0, 0, std::sin(angle), std::cos(angle), 0, 0, 0, 0, 1;
    return mat;
}

Matrix4d y_rotation_matrix3d(double angle)
{
    Matrix4d mat;
    mat << std::cos(angle), 0, std::sin(angle), 0, 0, 1, 0, 0, -std::sin(angle), 0, std::cos(angle), 0, 0, 0, 0, 1;
    return mat;
}

Matrix4d z_rotation_matrix3d(double angle)
{
    Matrix4d mat;
    mat << std::cos(angle), -std::sin(angle), 0, 0, std::sin(angle), std::cos(angle), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
    return mat;
}

Matrix4d translate_matrix3d(double x, double y, double z)
{
    Matrix4d mat;
    mat << 1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1;
    return mat;
}

#endif