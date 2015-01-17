#include "Context.h"
#include "InputHandler.h"
#include "HumanInputComponent.h"

HumanInputComponent::HumanInputComponent(GameObject &gameObject)
   : InputComponent(gameObject) {
}

HumanInputComponent::~HumanInputComponent() {
}

void HumanInputComponent::init() {
   Context::getInstance().getInputHandler().registerInputComponent(shared_from_this());
}
