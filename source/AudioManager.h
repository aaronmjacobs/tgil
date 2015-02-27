#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace FMOD {

class ChannelGroup;
class Sound;
class System;

} // namespace FMOD

typedef std::unordered_map<std::string, FMOD::Sound*> SoundMap;

struct ListenerAttributes {
   glm::vec3 position;
   glm::vec3 velocity;
   glm::vec3 forward;
   glm::vec3 up;
};

class AudioManager {
protected:
   FMOD::System *system;
   FMOD::ChannelGroup *musicGroup;
   FMOD::ChannelGroup *effectsGroup;

   SoundMap musicMap;
   SoundMap effectsMap;

   int numListeners;

   void release();

public:
   AudioManager();

   virtual ~AudioManager();

   void init();

   void update(ListenerAttributes *listeners, int numListeners);

   void loadMusic(const std::string &fileName);

   void loadSoundEffect(const std::string &fileName, float minDistance = 5.0f);

   void playMusic(const std::string &fileName);

   void playSoundEffect(const std::string &fileName);

   void playSoundEffect(const std::string &fileName, const glm::vec3 &pos, const glm::vec3 &vel = glm::vec3(0.0f));
};

#endif
