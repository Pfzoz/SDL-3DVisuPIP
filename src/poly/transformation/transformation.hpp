#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <eigen3/Eigen/Core>

Eigen::Matrix4d x_rotation_matrix3d(double angle);

Eigen::Matrix4d y_rotation_matrix3d(double angle);

Eigen::Matrix4d z_rotation_matrix3d(double angle);

Eigen::Matrix4d translate_matrix3d(double x, double y, double z);

#endif