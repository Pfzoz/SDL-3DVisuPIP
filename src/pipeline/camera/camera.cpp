#include "camera.hpp"

Camera::Camera()
{
    vrp = Eigen::Vector3d(0, 0, 0);
    focal_point = Eigen::Vector3d(0, 0, 1);
    u = Eigen::Vector3d(1, 0, 0);
    v = Eigen::Vector3d(0, 1, 0);
    n = Eigen::Vector3d(0, 0, -1);
}

// Setters
void Camera::set_vrp(Eigen::Vector3d vrp)
{
    this->vrp = vrp;
    this->n = vrp - focal_point;
    this->n.normalize();
    Eigen::Vector3d y(0, 1, 0);
    this->v = y - (y.dot(n)) * n;
    this->v.normalize();
    this->u = n.cross(this->v);
}

void Camera::set_focal_point(Eigen::Vector3d focal_point)
{
    this->focal_point = focal_point;
    this->set_vrp(this->vrp);
}

// Getters
Eigen::Vector3d Camera::get_vrp()
{
    return this->vrp;
}

Eigen::Vector3d Camera::get_focal_point()
{
    return this->focal_point;
}

Eigen::Vector3d Camera::get_v()
{
    return this->v;
}

Eigen::Vector3d Camera::get_n()
{
    return this->n;
}

Eigen::Vector3d Camera::get_u()
{
    return this->u;
}

Eigen::Matrix4d Camera::get_src_matrix()
{
    Eigen::Matrix4d src_matrix;
    src_matrix << this->u.x(), this->u.y(), this->u.z(), (-this->vrp).dot(this->u),
        this->v.x(), this->v.y(), this->v.z(), (-this->vrp).dot(this->v),
        this->n.x(), this->n.y(), this->n.z(), (-this->vrp).dot(this->n),
        0, 0, 0, 1;
    return src_matrix;
}