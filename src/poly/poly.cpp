#include "poly.hpp"

// Struct Functions

bool Poly::operator==(const Poly::Segment &lhs, const Poly::Segment &rhs)
{
    return (lhs.p1 == rhs.p1 && lhs.p2 == rhs.p2);
}

bool Poly::operator==(const Poly::Face &lhs, const Poly::Face &rhs)
{
    if (lhs.segments.size() != rhs.segments.size())
        return false;
    for (int i = 0; i < (int)lhs.segments.size(); i++)
    {
        if (lhs.segments[i] != rhs.segments[i])
            return false;
    }
    return true;
}

// Private Methods

// Constructors
Poly::Polyhedron::Polyhedron(){};

Poly::Polyhedron::Polyhedron(const std::vector<Segment> segments, const std::vector<Eigen::Vector3d> vertices, std::vector<Face> faces)
{
    this->vertices = vertices;
    this->segments = segments;
    this->faces = faces;
}

Poly::Polyhedron::Polyhedron(const Polyhedron &other)
{
    this->vertices = other.vertices;
    this->segments = other.segments;
    this->faces = other.faces;
    this->ambient_reflection_coefficient_b = other.ambient_reflection_coefficient_b;
    this->ambient_reflection_coefficient_g = other.ambient_reflection_coefficient_g;
    this->ambient_reflection_coefficient_r = other.ambient_reflection_coefficient_r;
    this->diffuse_recletion_coefficient_b = other.diffuse_recletion_coefficient_b;
    this->diffuse_recletion_coefficient_r = other.diffuse_recletion_coefficient_r;
    this->diffuse_recletion_coefficient_g = other.diffuse_recletion_coefficient_g;
    this->specular_reflection_coefficient_b = other.specular_reflection_coefficient_b;
    this->specular_reflection_coefficient_g = other.specular_reflection_coefficient_g;
    this->specular_reflection_coefficient_r = other.specular_reflection_coefficient_r;
    this->specular_exponent = other.specular_exponent;
}

// Getters
Eigen::Vector3d Poly::Polyhedron::get_vertex(int index)
{
    return this->vertices[index];
}

Poly::Segment Poly::Polyhedron::get_segment(int index)
{
    return this->segments[index];
}

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

uint Poly::Polyhedron::get_color()
{
    return 0xfffffff;
}

void Poly::Polyhedron::get_ambient_reflection_coefficients(double *r, double *g, double *b)
{
    *r = this->ambient_reflection_coefficient_r;
    *g = this->ambient_reflection_coefficient_g;
    *b = this->ambient_reflection_coefficient_b;
}

void Poly::Polyhedron::get_diffuse_reflection_coefficients(double *r, double *g, double *b)
{
    *r = this->diffuse_recletion_coefficient_r;
    *g = this->diffuse_recletion_coefficient_g;
    *b = this->diffuse_recletion_coefficient_b;
}

void Poly::Polyhedron::get_specular_reflection_coefficients(double *r, double *g, double *b)
{
    *r = this->specular_reflection_coefficient_r;
    *g = this->specular_reflection_coefficient_g;
    *b = this->specular_reflection_coefficient_b;
}

double Poly::Polyhedron::get_specular_exponent()
{
    return this->specular_exponent;
}

// Setters
void Poly::Polyhedron::set_ambient_reflection_coefficients(double r, double g, double b)
{
    this->ambient_reflection_coefficient_r = r;
    this->ambient_reflection_coefficient_g = g;
    this->ambient_reflection_coefficient_b = b;
}

void Poly::Polyhedron::set_diffuse_reflection_coefficients(double r, double g, double b)
{
    this->diffuse_recletion_coefficient_r = r;
    this->diffuse_recletion_coefficient_g = g;
    this->diffuse_recletion_coefficient_b = b;
}

void Poly::Polyhedron::set_specular_reflection_coefficients(double r, double g, double b)
{
    this->specular_reflection_coefficient_r = r;
    this->specular_reflection_coefficient_g = g;
    this->specular_reflection_coefficient_b = b;
}

void Poly::Polyhedron::set_specular_exponent(double specular_exponent)
{
    this->specular_exponent = specular_exponent;
}

void Poly::Polyhedron::move_to(Eigen::Vector3d pos)
{
    this->translate(pos.x() - this->get_center().x(), pos.y() - this->get_center().y(), pos.z() - this->get_center().z());
}

// Transformations
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
    {
        this->vertices[i](0) = result(0, i);
        this->vertices[i](1) = result(1, i);
        this->vertices[i](2) = result(2, i);
    }
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

void Poly::Polyhedron::transform(Eigen::Matrix4d matrix, std::vector<double> *h_factors)
{
    Eigen::MatrixXd hmatrix = this->get_hmatrix();
    Eigen::MatrixXd result = matrix * hmatrix;
    for (int i = 0; i < this->vertices.size(); i++)
        this->vertices[i] = result.block<3, 1>(0, i).cast<double>();
    if (h_factors != nullptr)
    {
        for (int i = 0; i < this->vertices.size(); i++)
            h_factors->push_back(result(3, i));
    }
}

int Poly::Polyhedron::find_segment(int p1, int p2, bool ignore_direction)
{
    if (!ignore_direction)
    {
        for (int i = 0; i < segments.size(); i++)
        {
            if (segments[i].p1 == p1 && segments[i].p2 == p2)
                return i;
        }
    }
    else
    {
        for (int i = 0; i < segments.size(); i++)
        {
            if ((segments[i].p1 == p1 && segments[i].p2 == p2) || (segments[i].p1 == p2 && segments[i].p2 == p1))
                return i;
        }
    }
    return -1;
}

void Poly::Polyhedron::print_faces()
{
    for (int i = 0; i < faces.size(); i++)
    {
        printf("Face %i\n", i);
        for (int j = 0; j < faces[i].segments.size(); j++)
        {
            printf("Segment(");
            printf("(%f,%f,%f),", vertices[segments[faces[i].segments[j]].p1].x(), vertices[segments[faces[i].segments[j]].p1].y(), vertices[segments[faces[i].segments[j]].p1].z());
            printf("(%f,%f,%f)", vertices[segments[faces[i].segments[j]].p2].x(), vertices[segments[faces[i].segments[j]].p2].y(), vertices[segments[faces[i].segments[j]].p2].z());
            printf(")\n");
        }
    }
}

void Poly::Polyhedron::print_segments()
{
    for (int i = 0; i < segments.size(); i++)
    {
        printf("Segment(");
        printf("(%f,%f,%f),", vertices[segments[i].p1].x(), vertices[segments[i].p1].y(), vertices[segments[i].p1].z());
        printf("(%f,%f,%f)", vertices[segments[i].p2].x(), vertices[segments[i].p2].y(), vertices[segments[i].p2].z());
        printf(")\n");
    }
}