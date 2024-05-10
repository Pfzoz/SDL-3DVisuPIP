#include "poly.hpp"

// Constructors
Poly::Polyhedron::Polyhedron(){};

Poly::Polyhedron::Polyhedron(std::vector<Segment> segments, std::vector<Eigen::Vector3d> vertices)
{
    this->segments = segments;
    this->vertices = vertices;
}

Poly::Polyhedron::Polyhedron(const Polyhedron& other)
{
    for (Eigen::Vector3d vector : other.vertices)
        this->vertices.push_back(vector);

    for (Poly::Segment segment : other.segments)
    {
        Poly::Segment new_segment;
        for (Eigen::Vector3d vector : this->vertices)
        {
            if (*segment.p1 == vector)
                new_segment.p1 = &vector;
            else if (*segment.p2 == vector)
                new_segment.p2 = &vector;
        }
        this->segments.push_back(new_segment);
    }
}

// Getters
Eigen::MatrixXd Poly::Polyhedron::get_matrix()
{
    Eigen::MatrixXd matrix(3, vertices.size());
    for (int i = 0; i < vertices.size(); i++)
        matrix.col(i) = vertices[i];
    return matrix;
}

Eigen::MatrixXd Poly::Polyhedron::get_hmatrix()
{
    Eigen::MatrixXd matrix(4, vertices.size());
    for (int i = 0; i < vertices.size(); i++)
    {
        matrix.col(i).head(3) = vertices[i];
        matrix(3, i) = 1;
    }
    return matrix;
}

Eigen::Vector3d Poly::Polyhedron::get_center()
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
    return Eigen::Vector3d((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
}

void Poly::Polyhedron::rotate(float x, float y, float z)
{
    Eigen::Matrix4d t_matrix = Eigen::Matrix4d::Identity();
    if (x != 0)
        t_matrix *= x_rotation_matrix3d(x);
    if (y != 0)
        t_matrix *= y_rotation_matrix3d(y);
    if (z != 0)
        t_matrix *= z_rotation_matrix3d(z);
    Eigen::MatrixXd result = t_matrix * this->get_hmatrix();
    for (int i = 0; i < this->vertices.size(); i++)
        this->vertices[i] = result.col(i).head(3);
}

void Poly::Polyhedron::translate(float x, float y, float z)
{
    Eigen::MatrixXd result = translate_matrix3d(x, y, z) * this->get_hmatrix();
    for (int i = 0; i < this->vertices.size(); i++)
        this->vertices[i] = result.col(i).head(3);
}

void Poly::Polyhedron::translate(Eigen::Vector3d v)
{
    this->translate(v.x(), v.y(), v.z());
}

void Poly::Polyhedron::mirror(bool x, bool y, bool z)
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