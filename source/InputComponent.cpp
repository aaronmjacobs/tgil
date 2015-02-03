#include "Context.h"
#include "FancyAssert.h"
#include "InputComponent.h"

InputComponent::InputComponent(GameObject &gameObject, const int playerNum)
   : Component(gameObject), playerNum(playerNum), inputValues({ 0 }) {
}

InputComponent::~InputComponent() {
}

void InputComponent::update() {
   if (playerNum >= 0) {
      inputValues = Context::getInstance().getInputHandler().getInputValues(playerNum);
   }
}

const int InputComponent::getPlayerNum() const {
   ASSERT(playerNum >= 0, "Invalid player num");
   return playerNum;
}

const InputValues& InputComponent::getInputValues() const {
   ASSERT(playerNum >= 0, "Invalid player num");
   return inputValues;
}
