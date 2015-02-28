#ifndef OBSERVER_H
#define OBSERVER_H

enum class Event {
   NOTHING,
   SET_SCENE,
   SCALE,
   STEP,
   JUMP
};

/**
 * An observer that can subscribe to be notified by subjects.
 */
template<class Entity>
class Observer {
public:
   virtual ~Observer() {}
   virtual void onNotify(const Entity &entity, Event event) = 0;
};

#endif
