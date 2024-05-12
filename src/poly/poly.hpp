#ifndef POLY_H
#define POLY_H

#include <vector>
#include <Eigen/Core>
#include "transformation/transformation.hpp"

namespace Poly
{
    struct Segment
    {
        size_t p1, p2;
    };

    struct Face
    {
        std::vector<size_t> segments;
    };

    bool operator==(const Segment &lhs, const Segment &rhs);
    bool operator==(const Face &lhs, const Face &rhs);

    class Polyhedron
    {
    public:
        std::vector<Eigen::Vector3d> vertices;
        std::vector<Segment> segments;
        std::vector<Face> faces;

        // Constructors
        Polyhedron();
        Polyhedron(const std::vector<Segment> segments, const std::vector<Eigen::Vector3d> vertices);
        Polyhedron(const Polyhedron& other);

        // Getters
        Eigen::Vector3d get_vertex(size_t index);
        Eigen::MatrixXd get_matrix();
        Eigen::MatrixXd get_hmatrix();
        Eigen::Vector3d get_center();

        // Transformations
        void rotate(float x = 0, float y = 0, float z = 0);
        void translate(float x = 0, float y = 0, float z = 0);
        void translate(Eigen::Vector3d v);
        void mirror(bool x, bool y, bool z);
    };

}

#endif