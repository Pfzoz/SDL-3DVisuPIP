#include <poly.hpp>
#include <Eigen/Core>
#include <SDL.h>
#include <vector>
#include <map>

Poly::Polyhedron wireframe(std::vector<SDL_FPoint> generatrix_points, int slices);