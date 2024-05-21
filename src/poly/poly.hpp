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
        size_t p1, p2;
        size_t successor = -1, predecessor = -1;
    };

    struct Face
    {
        std::vector<size_t> segments;
    };

    bool operator==(const Segment &lhs, const Segment &rhs);
    bool operator==(const Face &lhs, const Face &rhs);

    class Polyhedron
    {
    private:
    public:
        std::vector<Eigen::Vector3d> vertices;
        std::vector<Segment> segments;
        std::vector<Face> faces;

        // Constructors
        Polyhedron();
        Polyhedron(const std::vector<Segment> segments, const std::vector<Eigen::Vector3d> vertices, const std::vector<Face> faces);
        Polyhedron(const Polyhedron& other);

        // Getters
        Eigen::Vector3d get_vertex(size_t index);
        Poly::Segment get_segment(size_t index);
        Eigen::MatrixXd get_matrix();
        Eigen::MatrixXd get_hmatrix();
        Eigen::Vector3d get_center();
        size_t find_vertex(Eigen::Vector3d vertex);
        size_t find_segment(size_t p1, size_t p2, bool ignore_direction = false);
        size_t find_face(std::vector<size_t> segments);

        // Setters
        void move_to(Eigen::Vector3d pos);

        // Transformations
        void rotate(float x = 0, float y = 0, float z = 0);
        void translate(float x = 0, float y = 0, float z = 0);
        void translate(Eigen::Vector3d v);
        void mirror(bool x, bool y, bool z);
        void transform(Eigen::Matrix4d matrix, std::vector<double> *h_factors = nullptr);

        // Repr
        void print_faces();

    };

}

#endif