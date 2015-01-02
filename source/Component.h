#ifndef COMPONENT_H
#define COMPONENT_H

#include "GameObject.h"
#include "Types.h"

template<class Class, class NullClass>
class Component {
private:
   static UPtr<GameObject> nullGameObject;
   static SPtr<Class> nullComponent;

protected:
   GameObject &gameObject;

   static GameObject& getNullGameObject();

public:
   Component(GameObject& gameObject)
      : gameObject(gameObject) {}

   virtual ~Component() {}

   static SPtr<Class> getNullComponent();
};

template<class Class, class NullClass>
UPtr<GameObject> Component<Class, NullClass>::nullGameObject;

template<class Class, class NullClass>
SPtr<Class> Component<Class, NullClass>::nullComponent;

template<class Class, class NullClass>
inline GameObject& Component<Class, NullClass>::getNullGameObject() {
   if (!nullGameObject) {
      nullGameObject = std::move(UPtr<GameObject>(new GameObject));
   }

   return *nullGameObject;
}

template<class Class, class NullClass>
inline SPtr<Class> Component<Class, NullClass>::getNullComponent() {
   if (!nullComponent) {
      nullComponent = std::make_shared<NullClass>();
   }

   return nullComponent;
}

#endif
