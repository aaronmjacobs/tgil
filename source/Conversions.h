#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

inline glm::vec3 toGlm(const btVector3 &vec) {
   return glm::vec3(vec.x(), vec.y(), vec.z());
}

inline glm::quat toGlm(const btQuaternion &quat) {
   return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

inline btVector3 toBt(const glm::vec3 &vec) {
   return btVector3(vec.x, vec.y, vec.z);
}

inline btQuaternion toBt(const glm::quat &quat) {
   return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

#endif
