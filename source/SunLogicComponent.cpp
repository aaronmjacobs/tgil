#include "Context.h"
#include "GameObject.h"
#include "LightComponent.h"
#include "SunLogicComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

SunLogicComponent::SunLogicComponent(GameObject &gameObject)
   : LogicComponent(gameObject) {
}

SunLogicComponent::~SunLogicComponent() {
}

void SunLogicComponent::tick(const float dt) {
   float t = Context::getInstance().getRunningTime() * 2.0f * glm::pi<float>();
   t /= 60.0f;
   float x = glm::cos(t);
   float y = glm::sin(t);
   glm::vec3 pos(x, y, 0.0f);
   gameObject.getLightComponent().setDirection(-pos);
}
