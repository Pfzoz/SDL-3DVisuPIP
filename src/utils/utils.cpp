#include "utils.hpp"

void print_matrix(Eigen::MatrixXd mat)
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

void print_vector(Eigen::VectorXd vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        printf("%.6f\t", vec(i));
    }
    printf("\n");
}