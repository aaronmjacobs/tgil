#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include "Component.h"
#include "Observer.h"

#include <string>
#include <map>

class SoundGroup;

typedef std::map<Event, const SoundGroup*> EventSoundMap;

class AudioComponent : public Component, public Observer<GameObject> {
protected:
   EventSoundMap eventSoundMap;

public:
   AudioComponent(GameObject &gameObject);

   virtual ~AudioComponent();

   void registerSoundEvent(Event event, const SoundGroup &soundGroup);

   virtual void onNotify(const GameObject &gameObject, Event event);
};

#endif
