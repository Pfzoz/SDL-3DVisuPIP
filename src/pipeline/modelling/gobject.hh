#ifndef GOBJECT_H
#define GOBJECT_H

#include <vector>
#include "../../eigen-3.4.0/Eigen/Core"
#include "geometry.hh"
#include "transformation.hh"

using Eigen::Matrix3d;
using Eigen::Matrix4d;
using Eigen::MatrixXd;
using Eigen::Vector2d;
using Eigen::Vector3d;

class GObject3d
{
public:
    std::vector<Vector3d> vertices;
    std::vector<Segment3d> segments;

    GObject3d(){};
    GObject3d(std::vector<Segment3d> segments, std::vector<Vector3d> vertices)
    {
        this->segments = segments;
        this->vertices = vertices;
    }

    MatrixXd get_matrix()
    {
        MatrixXd matrix(3, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
            matrix.col(i) = vertices[i];
        return matrix;
    }

    MatrixXd get_hmatrix()
    {
        MatrixXd matrix(4, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
        {
            matrix.col(i).head(3) = vertices[i];
            matrix(3, i) = 1;
        }
        return matrix;
    }

    void rotate(float x = 0, float y = 0, float z = 0)
    {
        Matrix4d t_matrix = Matrix4d::Identity();
        if (x != 0)
            t_matrix *= x_rotation_matrix3d(x);
        if (y != 0)
            t_matrix *= y_rotation_matrix3d(y);
        if (z != 0)
            t_matrix *= z_rotation_matrix3d(z);
        MatrixXd result = t_matrix * this->get_hmatrix();
        for (int i = 0; i < this->vertices.size(); i++)
            this->vertices[i] = result.col(i).head(3);
    }

    void translate(float x = 0, float y = 0, float z = 0)
    {
        MatrixXd result = translate_matrix3d(x, y, z) * this->get_hmatrix();
        for (int i = 0; i < this->vertices.size(); i++)
            this->vertices[i] = result.col(i).head(3);
    }

    void translate(Vector3d v)
    {
        this->translate(v.x(), v.y(), v.z());
    }

    void mirror(bool x, bool y, bool z)
    {
        if (x)
        {
            for (int i = 0; i < this->vertices.size(); i++)
                this->vertices[i].x() = -this->vertices[i].x();
        }
        if (y)
        {
            for (int i = 0; i < this->vertices.size(); i++)
                this->vertices[i].y() = -this->vertices[i].y();
        }
        if (z)
        {
            for (int i = 0; i < this->vertices.size(); i++)
                this->vertices[i].z() = -this->vertices[i].z();
        }
    }

    Vector3d get_center()
    {
        float min_x, max_x, min_y, max_y, min_z, max_z;
        min_x = this->vertices[0].x();
        max_x = this->vertices[0].x();
        min_y = this->vertices[0].y();
        max_y = this->vertices[0].y();
        min_z = this->vertices[0].z();
        max_z = this->vertices[0].z();
        for (int i = 1; i < this->vertices.size(); i++)
        {
            if (this->vertices[i].x() < min_x)
                min_x = this->vertices[i].x();
            if (this->vertices[i].x() > max_x)
                max_x = this->vertices[i].x();
            if (this->vertices[i].y() < min_y)
                min_y = this->vertices[i].y();
            if (this->vertices[i].y() > max_y)
                max_y = this->vertices[i].y();
            if (this->vertices[i].z() < min_z)
                min_z = this->vertices[i].z();
            if (this->vertices[i].z() > max_z)
                max_z = this->vertices[i].z();
        }
        return Vector3d((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
    }
};

class GObject2d
{
public:
    std::vector<Vector2d> vertices;
    std::vector<Segment2d> segments;

    GObject2d(){};
    GObject2d(std::vector<Segment2d> segments, std::vector<Vector2d> vertices)
    {
        this->segments = segments;
        this->vertices = vertices;
    }

    MatrixXd get_matrix()
    {
        MatrixXd matrix(2, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
            matrix.col(i) = vertices[i];
        return matrix;
    }

    MatrixXd get_hmatrix()
    {
        MatrixXd matrix(3, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
        {
            matrix.col(i).head(2) = vertices[i];
            matrix(2, i) = 1;
        }
        return matrix;
    }

    void trans_rotate(float x = 0, float y = 0)
    {
    }
};

#endif