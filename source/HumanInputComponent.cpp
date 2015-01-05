#include "Context.h"
#include "InputHandler.h"
#include "HumanInputComponent.h"

HumanInputComponent::~HumanInputComponent() {
}

void HumanInputComponent::pollInput(GameObject &gameObject) {
   inputValues = Context::getInstance().getInputHandler().getInputValues(0); // TODO Multiple players
}

const InputValues& HumanInputComponent::getInputValues(GameObject &gameObject) {
   return inputValues;
}
