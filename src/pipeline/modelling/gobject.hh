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
    void destroy()
    {
        for (int i = 0; i < vertices.size(); i++)
            delete vertices[i];
    };

    MatrixXd get_matrix()
    {
        MatrixXd matrix(3, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
            matrix.col(i) = *vertices[i];
        return matrix;
    }

    MatrixXd get_hmatrix()
    {
        MatrixXd matrix(4, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
            matrix.col(i).head(3) = *vertices[i];
        return matrix;
    }

    void trans_rotate(float x = 0, float y = 0, float z = 0)
    {
        if (x != 0)
        {
        }
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
    void destroy()
    {
        for (int i = 0; i < vertices.size(); i++)
            delete vertices[i];
    };

    MatrixXd get_matrix()
    {
        MatrixXd matrix(2, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
            matrix.col(i) = *vertices[i];
        return matrix;
    }

    MatrixXd get_hmatrix()
    {
        MatrixXd matrix(3, vertices.size());
        for (int i = 0; i < vertices.size(); i++)
            matrix.col(i).head(2) = *vertices[i];
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