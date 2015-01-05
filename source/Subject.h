#ifndef SUBJECT_H
#define SUBJECT_H

#include "Observer.h"
#include "Types.h"

#include <vector>

/**
 * A subject that observers can subscribe to. Observers can be added and remove, though explicity removal is not necessary.
 * Any observers that are not explicitly removed, but which are deleted, will automatically be removed from any subjects they are subscribed to.
 */
template<class Entity>
class Subject {
private:
   std::vector<WPtr<Observer<Entity>>> observers;

protected:
   void notify(const Entity &entity, Event event) {
      for (typename std::vector<WPtr<Observer<Entity>>>::iterator itr = observers.begin(); itr != observers.end();) {
         SPtr<Observer<Entity>> observer = itr->lock();
         if (observer) {
            observer->onNotify(entity, event);
            ++itr;
         } else {
            itr = observers.erase(itr);
         }
      }
   }

public:
   virtual ~Subject() {}

   void addObserver(SPtr<Observer<Entity>> observer) {
      observers.push_back(observer);
   }

   void removeObserver(SPtr<Observer<Entity>> observer) {
      observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
   }
};

#endif
