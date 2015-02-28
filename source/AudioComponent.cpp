#include "AudioComponent.h"
#include "AudioManager.h"
#include "Context.h"
#include "GameObject.h"

AudioComponent::AudioComponent(GameObject &gameObject)
   : Component(gameObject) {
}

AudioComponent::~AudioComponent() {
}

void AudioComponent::registerSoundEvent(Event event, const SoundGroup &soundGroup) {
   eventSoundMap[event] = &soundGroup;
}

void AudioComponent::onNotify(const GameObject &gameObject, Event event) {
   if (eventSoundMap.count(event)) {
      Context::getInstance().getAudioManager().play(*eventSoundMap.at(event), gameObject.getPosition());
   }
}
