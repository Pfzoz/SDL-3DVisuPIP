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
    Eigen::Matrix4d get_src_matrix();

    // Setters
    void set_vrp(Eigen::Vector3d vrp);
    void set_focal_point(Eigen::Vector3d focal_point);
};