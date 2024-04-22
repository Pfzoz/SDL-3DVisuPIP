#ifndef GOBJECT_H
#define GOBJECT_H

#include <vector>
#include "../../eigen-3.4.0/Eigen/Core"
#include "geometry.hh"
#include "transformation.hh"

using Eigen::MatrixXd;
using Eigen::Matrix3d;
using Eigen::Matrix4d;
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
        if (x != 0)
        {
        }
    }
};

#endif