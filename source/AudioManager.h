#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <string>
#include <unordered_map>

namespace FMOD {

class ChannelGroup;
class Sound;
class System;

} // namespace FMOD

typedef std::unordered_map<std::string, FMOD::Sound*> SoundMap;

enum class SoundType {
   Music,
   SoundEffect
};

class AudioManager {
protected:
   FMOD::System *system;
   FMOD::ChannelGroup *musicGroup;
   FMOD::ChannelGroup *effectsGroup;

   SoundMap musicMap;
   SoundMap effectsMap;

   void release();

public:
   AudioManager();

   virtual ~AudioManager();

   void init();

   void update();

   void loadSound(const std::string &fileName, SoundType type, bool threeDimensional = false);

   void playMusic(const std::string &fileName);

   void playSoundEffect(const std::string &fileName);
};

#endif
