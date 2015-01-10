#include "Conversions.h"
#include "GameObject.h"
#include "GameObjectMotionState.h"

GameObjectMotionState::GameObjectMotionState(GameObject &gameObject)
   : gameObject(gameObject) {
}

GameObjectMotionState::~GameObjectMotionState() {
}

void GameObjectMotionState::getWorldTransform(btTransform &worldTrans) const {
   worldTrans.setOrigin(toBt(gameObject.getPosition()));
   worldTrans.setRotation(toBt(gameObject.getOrientation()));
}

void GameObjectMotionState::setWorldTransform(const btTransform &worldTrans) {
   gameObject.setPosition(toGlm(worldTrans.getOrigin()));
   gameObject.setOrientation(toGlm(worldTrans.getRotation()));
}
