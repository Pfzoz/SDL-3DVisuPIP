#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include "eigen-3.4.0/Eigen/Core"

using Eigen::MatrixXd;
using Eigen::VectorXd;
void print_matrix(MatrixXd mat)
{
    for (int i = 0; i < mat.rows(); i++)
    {
        for (int j = 0; j < mat.cols(); j++)
        {
            printf("%.6f\t", mat(i, j));
        }
        printf("\n");
    }
}

void print_vector(VectorXd vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        printf("%.6f\t", vec(i));
    }
    printf("\n");
}

#endif