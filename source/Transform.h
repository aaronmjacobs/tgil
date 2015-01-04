#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
   glm::quat orientation;
   glm::vec3 position;
   glm::vec3 scale;
};

#endif
