#ifndef COMPONENT_H
#define COMPONENT_H

#include "Types.h"

template<class Class, class NullClass>
class Component {
private:
   static SPtr<Class> nullComponent;

public:
   virtual ~Component() {}

   static SPtr<Class> getNullComponent();
};

template<class Class, class NullClass>
SPtr<Class> Component<Class, NullClass>::nullComponent;

template<class Class, class NullClass>
inline SPtr<Class> Component<Class, NullClass>::getNullComponent() {
   if (!nullComponent) {
      nullComponent = std::make_shared<NullClass>();
   }

   return nullComponent;
}

#endif
