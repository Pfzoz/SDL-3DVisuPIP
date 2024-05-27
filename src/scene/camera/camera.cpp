#include "camera.hpp"

Camera::Camera()
{
    vrp = Eigen::Vector3d(0, 0, 0);
    set_focal_point(Eigen::Vector3d(1, 0, 0));
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

void Camera::translate(float x, float y, float z)
{
    this->vrp = this->vrp + Eigen::Vector3d(x, y, z);
    this->focal_point = this->focal_point + Eigen::Vector3d(x, y, z);
    this->set_vrp(this->vrp);
}

void Camera::rotate(float x, float y, float z)
{
    Eigen::Matrix4d r_matrix;
    Eigen::Vector4d h_vrp, h_focal_point;
    h_vrp << this->vrp.x(), this->vrp.y(), this->vrp.z(), 1;
    h_focal_point << this->focal_point.x(), this->focal_point.y(), this->focal_point.z(), 1;
    r_matrix << std::cos(z)*std::cos(y), std::cos(z)*std::sin(y)*std::sin(x) - std::sin(z)*std::cos(x), std::cos(z)*std::sin(y)*std::cos(x) + std::sin(z)*std::sin(x), 0,
                std::sin(z)*std::cos(y), std::sin(z)*std::sin(y)*std::sin(x) + std::cos(z)*std::cos(x), std::sin(z)*std::sin(y)*std::cos(x) - std::cos(z)*std::sin(x), 0,
                -std::sin(y), std::cos(y)*std::sin(x), std::cos(y)*std::cos(x), 0,
                0, 0, 0, 1;
    this->translate(-this->vrp.x(), -this->vrp.y(), -this->vrp.z());
    this->vrp = (r_matrix * h_vrp).col(0).head(3);
    this->focal_point = (r_matrix * h_focal_point).col(0).head(3);
    this->set_vrp(this->vrp);
    this->translate(this->vrp.x(), this->vrp.y(), this->vrp.z());
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