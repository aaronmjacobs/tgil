#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
   glm::quat orientation;
   glm::vec3 position;
   glm::vec3 scale;

   Transform()
      : orientation(), position(0.0f), scale(1.0f) {}
};

#endif
