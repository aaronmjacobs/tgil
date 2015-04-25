#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

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

class SoundGroup {
protected:
   const std::vector<std::string> soundFiles;
   const bool stream;
   const bool threeDimensional;
   const float minDistance;

   SoundGroup(const std::vector<std::string> &soundFiles, bool stream, bool threeDimensional, float minDistance);

public:
   // Music
   static const SoundGroup STRIFE;

   // Sound effects
   static const SoundGroup JUMP;
   static const SoundGroup STEP;
   static const SoundGroup DIE;
   static const SoundGroup EXPLOSION;
   static const SoundGroup SHOVE;
   static const SoundGroup THROW;

   virtual ~SoundGroup();

   const std::string& getSoundFile() const;

   const std::vector<std::string>& getSoundFiles() const {
      return soundFiles;
   }

   bool isStream() const {
      return stream;
   }

   bool isThreeDimensional() const {
      return threeDimensional;
   }

   float getMinDistance() const {
      return minDistance;
   }
};

class AudioManager {
protected:
   FMOD::System *system;
   FMOD::ChannelGroup *musicGroup;
   FMOD::ChannelGroup *effectsGroup;

   SoundMap musicMap;
   SoundMap effectsMap;

   SoundMap soundMap;

   int numListeners;

   void release();

   void load(const SoundGroup &soundGroup);

public:
   AudioManager();

   virtual ~AudioManager();

   void init();

   void update(ListenerAttributes *listeners, int numListeners);

   void play(const SoundGroup &soundGroup, const glm::vec3 &pos = glm::vec3(0.0f), const glm::vec3 &vel = glm::vec3(0.0f));
};

#endif
