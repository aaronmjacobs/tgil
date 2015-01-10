#include "Context.h"
#include "InputHandler.h"
#include "HumanInputComponent.h"

HumanInputComponent::HumanInputComponent(GameObject &gameObject)
   : InputComponent(gameObject) {
}

HumanInputComponent::~HumanInputComponent() {
}

void HumanInputComponent::pollInput() {
   inputValues = Context::getInstance().getInputHandler().getInputValues(0); // TODO Multiple players
}

const InputValues& HumanInputComponent::getInputValues() {
   return inputValues;
}
