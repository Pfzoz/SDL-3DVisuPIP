#include <Eigen/Core>
#include <Eigen/Geometry>
class Camera
{
private:
    Eigen::Vector3d vrp, focal_point, u, v, n;

public:
    Camera();

    // Getters
    Eigen::Vector3d get_vrp();
    Eigen::Vector3d get_focal_point();
    Eigen::Vector3d get_v();
    Eigen::Vector3d get_n();
    Eigen::Vector3d get_u();
    Eigen::Matrix4d get_src_matrix();

    // Setters
    void set_vrp(Eigen::Vector3d vrp);
    void set_focal_point(Eigen::Vector3d focal_point);
    void translate(float x = 0, float y = 0, float z = 0);
    void rotate(float x = 0, float y = 0, float z = 0);
};