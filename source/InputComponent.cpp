#include "Context.h"
#include "FancyAssert.h"
#include "InputComponent.h"

InputComponent::InputComponent(GameObject &gameObject, const int deviceNum)
   : Component(gameObject), deviceNum(deviceNum), inputValues({ 0 }) {
}

InputComponent::~InputComponent() {
}

void InputComponent::update() {
   if (deviceNum >= 0) {
      inputValues = Context::getInstance().getInputHandler().getInputValues(deviceNum);
   }
}

const int InputComponent::getDeviceNum() const {
   ASSERT(deviceNum >= 0, "Invalid device num");
   return deviceNum;
}

const InputValues& InputComponent::getInputValues() const {
   ASSERT(deviceNum >= 0, "Invalid device num");
   return inputValues;
}
