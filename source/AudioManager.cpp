#include "AudioManager.h"
#include "FancyAssert.h"
#include "IOUtils.h"
#include "LogHelper.h"

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>

#include <sstream>

namespace {

const int MAX_CHANNELS = 64;
const float MAX_DISTANCE = 10000.0f;
const float MUSIC_VOLUME = 0.8f;

enum class ErrorLevel {
   Warning,
   Error,
   Fatal
};

bool check(FMOD_RESULT result, ErrorLevel errorLevel = ErrorLevel::Warning) {
   if (result != FMOD_OK) {
      switch (errorLevel) {
         case ErrorLevel::Warning:
            LOG_WARNING("FMOD error: " << FMOD_ErrorString(result));
            break;
         case ErrorLevel::Error:
            LOG_ERROR("FMOD error: " << FMOD_ErrorString(result));
            break;
         case ErrorLevel::Fatal:
            LOG_FATAL("FMOD error: " << FMOD_ErrorString(result));
            break;
         default:
            ASSERT(false, "Invalid error level");
      }
   }

   return result == FMOD_OK;
}

FMOD_RESULT F_CALLBACK debugCallback(FMOD_DEBUG_FLAGS flags, const char *file, int line, const char *func, const char *message) {
   std::stringstream ss;

   ss << "FMOD at " << func << ": " << message;

   // Remove any trailing newline
   std::string output = ss.str();
   size_t endpos = output.find_last_not_of(" \t\r\n");
   if (std::string::npos != endpos) {
      output = output.substr(0, endpos + 1);
   }

   switch (flags) {
      case FMOD_DEBUG_LEVEL_NONE:
         break;
      case FMOD_DEBUG_LEVEL_ERROR:
         LOG_ERROR(output);
         break;
      case FMOD_DEBUG_LEVEL_WARNING:
         LOG_WARNING(output);
         break;
      case FMOD_DEBUG_LEVEL_LOG:
      default:
         LOG_INFO(output);
         break;
   }

   return FMOD_OK;
}

FMOD_VECTOR toFmod(const glm::vec3 &vector) {
   return { vector.x, vector.y, vector.z };
}

} // namespace

AudioManager::AudioManager()
   : system(nullptr), musicGroup(nullptr), effectsGroup(nullptr), numListeners(1) {
}

AudioManager::~AudioManager() {
   release();
}

void AudioManager::release() {
   if (system) {
      FMOD_RESULT result = system->release();
      system = nullptr;

      if (result != FMOD_OK) {
         LOG_WARNING("Unable to release FMOD system");
      }
   }
}

void AudioManager::init() {
   ASSERT(!system, "Audio system already initialized");

   FMOD_RESULT result = FMOD::System_Create(&system);
   if (result != FMOD_OK) {
      release();
      LOG_ERROR("Unable to create audio system");
      return;
   }

   unsigned int version;
   result = system->getVersion(&version);
   if (result != FMOD_OK) {
      LOG_ERROR("Unable to get FMOD version");
   } else if (version != FMOD_VERSION) {
      LOG_ERROR("FMOD lib version " << version << " doesn't match header version " << FMOD_VERSION);
   }

   RUN_DEBUG(check(FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_WARNING, FMOD_DEBUG_MODE_CALLBACK, debugCallback));)

   result = system->init(MAX_CHANNELS, FMOD_INIT_3D_RIGHTHANDED, nullptr);
   if (result != FMOD_OK) {
      LOG_ERROR("Unable to initialize audio system");
      release();
      return;
   }

   check(system->createChannelGroup("music", &musicGroup));
   check(system->createChannelGroup("effects", &effectsGroup));

   check(musicGroup->setVolume(MUSIC_VOLUME));
}

void AudioManager::update(ListenerAttributes *listeners, int numListeners) {
   ASSERT(system, "Audio system not initialized");
   ASSERT(numListeners > 0 && numListeners < 5, "Invalid number of listeners");

   if (numListeners > 0 && numListeners < 5) {
      if (this->numListeners != numListeners) {
         this->numListeners = numListeners;
         check(system->set3DNumListeners(numListeners));
      }

      for (int i = 0; i < numListeners; ++i) {
         FMOD_VECTOR position = toFmod(listeners[i].position);
         FMOD_VECTOR velocity = toFmod(listeners[i].velocity);
         FMOD_VECTOR forward = toFmod(listeners[i].forward);
         FMOD_VECTOR up = toFmod(listeners[i].up);

         check(system->set3DListenerAttributes(i, &position, &velocity, &forward, &up));
      }
   }

   check(system->update());
}

void AudioManager::loadMusic(const std::string &fileName) {
   ASSERT(system, "Audio system not initialized");

   FMOD::Sound *sound;
   if (check(system->createStream(IOUtils::dataPath(fileName).c_str(), FMOD_LOOP_NORMAL, nullptr, &sound))) {
      musicMap[fileName] = sound;
   }
}

void AudioManager::loadSoundEffect(const std::string &fileName, float minDistance) {
   ASSERT(system, "Audio system not initialized");

   FMOD::Sound *sound;
   if (check(system->createSound(IOUtils::dataPath(fileName).c_str(), FMOD_3D, nullptr, &sound))) {
      sound->set3DMinMaxDistance(minDistance, MAX_DISTANCE);
      effectsMap[fileName] = sound;
   }
}

void AudioManager::playMusic(const std::string &fileName) {
   ASSERT(system, "Audio system not initialized");

   if (!musicMap.count(fileName)) {
      LOG_WARNING("No music with file name: " << fileName);
      return;
   }

   FMOD::Channel *channel = nullptr;
   check(system->playSound(musicMap.at(fileName), musicGroup, false, &channel));
   if (channel) {
      check(channel->setLoopCount(-1));
   }
}

void AudioManager::playSoundEffect(const std::string &fileName) {
   ASSERT(system, "Audio system not initialized");

   if (!effectsMap.count(fileName)) {
      LOG_WARNING("No sound effect with file name: " << fileName);
      return;
   }

   check(system->playSound(effectsMap.at(fileName), effectsGroup, false, nullptr));
}

void AudioManager::playSoundEffect(const std::string &fileName, const glm::vec3 &pos, const glm::vec3 &vel) {
   ASSERT(system, "Audio system not initialized");

   if (!effectsMap.count(fileName)) {
      LOG_WARNING("No sound effect with file name: " << fileName);
      return;
   }

   FMOD::Channel *channel = nullptr;
   check(system->playSound(effectsMap.at(fileName), effectsGroup, true, &channel));

   if (!channel) {
      return;
   }

   FMOD_VECTOR fmodPos = toFmod(pos);
   FMOD_VECTOR fmodVel = toFmod(vel);
   check(channel->set3DAttributes(&fmodPos, &fmodVel));
   check(channel->setPaused(false));
}
