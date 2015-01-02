#include "Context.h"
#include "InputHandler.h"
#include "HumanInputComponent.h"

HumanInputComponent::HumanInputComponent(GameObject &gameObject)
   : InputComponent(gameObject) {
}

HumanInputComponent::~HumanInputComponent() {
}

void HumanInputComponent::pollInput(const Context &context, GameObject &gameObject) {
   inputValues = context.getInputHandler().getInputValues(0); // TODO Multiple players
}

const InputValues& HumanInputComponent::getInputValues(GameObject &gameObject) {
   return inputValues;
}
