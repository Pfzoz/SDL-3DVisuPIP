#ifndef POLY_H
#define POLY_H

#include <vector>
#include <eigen3/Eigen/Core>
#include "transformation/transformation.hpp"

namespace Poly
{
    struct Segment
    {
        Eigen::Vector3d *p1;
        Eigen::Vector3d *p2;
    };

    class Polyhedron
    {
    public:
        std::vector<Eigen::Vector3d> vertices;
        std::vector<Segment> segments;

        // Constructors
        Polyhedron();
        Polyhedron(std::vector<Segment> segments, std::vector<Eigen::Vector3d> vertices);
        Polyhedron(const Polyhedron& other);

        // Getters
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