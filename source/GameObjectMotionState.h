#ifndef GAME_OBJECT_MOTION_STATE_H
#define GAME_OBJECT_MOTION_STATE_H

#include <bullet/btBulletDynamicsCommon.h>

class GameObject;

class GameObjectMotionState : public btMotionState {
protected:
   GameObject &gameObject;

public:
   GameObjectMotionState(GameObject &gameObject);

   virtual ~GameObjectMotionState();

   virtual void getWorldTransform(btTransform &worldTrans) const;

   virtual void setWorldTransform(const btTransform &worldTrans);
};

#endif
