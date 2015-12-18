#ifndef RAY_H
#define RAY_H

#include "Vec3.h"
#include "Mesh.h"
#include <cmath>

class Ray {
 public:
  Vec3<float> origin;
  Vec3<float> direction;

  Ray(float vx, float vy, float vz, float lx, float ly, float lz);
  int intersect(Vec3<float> v0, Vec3<float> v1, Vec3<float> v2);
};

#endif
