#include "Ray.h"

Ray::Ray(float vx, float vy, float vz, float lx, float ly, float lz){
  origin = Vec3<float>(vx, vy, vz);
  direction = Vec3<float>(lx - vx, ly - vy, lz - vz);
  direction.normalize();
};

int Ray::intersect(Vec3<float> v0, Vec3<float> v1, Vec3<float> v2){

  // Implemantation found on the internet
  // At this site here: http://geomalgorithms.com/a06-_intersect-2.html

  // // Calculate the normal of the triangle
  // Vec3<float> u = v1 - v0;
  // Vec3<float> v = v2 - v0;

  // Vec3<float> norm = cross(u, v);
  // norm.normalize();

  // // Verify if the Ray intersects the plane where the triangle is
  // if (dot(direction, norm) == 0)
  //   return 0;

  // //Find the point of intersection with the plane
  // float d_int = dot(v0 - origin, norm)/dot(direction, norm);
  // Vec3<float> p_int = d_int * direction + origin;

  // // Verify if the intersection is inside the triangle
  // Vec3<float> w = p_int - v0;

  // float denominator = pow(dot(u, v), 2) - dot(u, u)*dot(v, v);
  // float s_int = dot(u, v)*dot(w, v) - dot(v, v)*dot(w, u);
  // s_int /= denominator;
  // float t_int = dot(u, v)*dot(w, u) - dot(u, u)*dot(w, v);
  // t_int /= t_int;

  // if (t_int <= 0)
  //   return 0;
  // if (s_int <= 0)
  //   return 0;
  // if ((t_int + s_int ) > 1)
  //   return 0;

  // return 1;

  // End of implemantation found on the internet

  // Implementation of the amgorithm proposed by teacher

  //Calculate the normal of the triangle
  Vec3<float> e0 = v1 - v0;
  Vec3<float> e1 = v2 - v0;

  Vec3<float> norm = cross(e0, e1);
  norm.normalize();

  Vec3<float> q = cross(direction, e1);
  float a = dot(e0, q);
  // std::cerr << "Value of a: " << a << std::endl;

  float epsilon = 0.1f;
  if ((dot(norm, direction) >= 0) || (std::abs(a) < epsilon))
    return 0;

  Vec3<float> s = (origin - v0)/a;
  Vec3<float> r = cross(s, e0);

  float b0 = dot(s, q);
  float b1 = dot(r, direction);
  float b2 = 1 - b0 - b1;

  if ((b0<0) || (b1<0) || (b2<0))
    return 0;

  float t = dot(e1, r);

  if (t>= 0)
    return 1;

  return 0;

  // End of implementation of the amgorithm proposed by teacher
};
