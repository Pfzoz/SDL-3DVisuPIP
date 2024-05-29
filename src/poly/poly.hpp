#ifndef POLY_H
#define POLY_H

#include <vector>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "transformation/transformation.hpp"
#include <utils.hpp>

namespace Poly
{
    struct Segment
    {
        int p1, p2;
        int successor = 0, predecessor = 0;
    };

    struct Face
    {
        std::vector<int> segments;
        bool draw_flag = true;
    };

    bool operator==(const Segment &lhs, const Segment &rhs);
    bool operator==(const Face &lhs, const Face &rhs);

    class Polyhedron
    {
    private:
    public:
        double ambient_reflection_coefficient_r = 0.1f;
        double ambient_reflection_coefficient_g = 0.1f;
        double ambient_reflection_coefficient_b = 0.1f;
        double diffuse_recletion_coefficient_r = 0.5f;
        double diffuse_recletion_coefficient_g = 0.5f;
        double diffuse_recletion_coefficient_b = 0.5f;
        double specular_reflection_coefficient_r = 0.5f;
        double specular_reflection_coefficient_g = 0.5f;
        double specular_reflection_coefficient_b = 0.5f;       
        double specular_exponent = 2.15f;
        std::vector<Eigen::Vector3d> vertices;
        std::vector<Segment> segments;
        std::vector<Face> faces;

        // Constructors
        Polyhedron();
        Polyhedron(const std::vector<Segment> segments, const std::vector<Eigen::Vector3d> vertices, const std::vector<Face> faces);
        Polyhedron(const Polyhedron &other);

        // Getters
        Eigen::Vector3d get_vertex(int index);
        Poly::Segment get_segment(int index);
        Eigen::MatrixXd get_matrix();
        Eigen::MatrixXd get_hmatrix();
        Eigen::Vector3d get_center();
        int find_vertex(Eigen::Vector3d vertex);
        int find_segment(int p1, int p2, bool ignore_direction = false);
        int find_face(std::vector<int> segments);
        void get_ambient_reflection_coefficients(double *r, double *g, double *b);
        void get_diffuse_reflection_coefficients(double *r, double *g, double *b);
        void get_specular_reflection_coefficients(double *r, double *g, double *b);
        double get_specular_exponent();
        uint get_color();

        // Setters
        void move_to(Eigen::Vector3d pos);
        void set_ambient_reflection_coefficients(double r, double g, double b);
        void set_diffuse_reflection_coefficients(double r, double g, double b);
        void set_specular_reflection_coefficients(double r, double g, double b);
        void set_specular_exponent(double specular_exponent);

        // Transformations
        void rotate(float x = 0, float y = 0, float z = 0);
        void translate(float x = 0, float y = 0, float z = 0);
        void translate(Eigen::Vector3d v);
        void mirror(bool x, bool y, bool z);
        void transform(Eigen::Matrix4d matrix, std::vector<double> *h_factors = nullptr);
        void scale(float x = 1, float y = 1, float z = 1);

        // Repr
        void print_segments();
        void print_faces();
    };

}

#endif