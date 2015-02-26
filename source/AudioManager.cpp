#include "AudioManager.h"
#include "FancyAssert.h"
#include "IOUtils.h"
#include "LogHelper.h"

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>

#include <sstream>

namespace {

const int MAX_CHANNELS = 64;
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

} // namespace

AudioManager::AudioManager()
   : system(nullptr) {
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

void AudioManager::update() {
   ASSERT(system, "Audio system not initialized");

   check(system->update());
}

void AudioManager::loadSound(const std::string &fileName, SoundType type, bool threeDimensional) {
   ASSERT(system, "Audio system not initialized");

   FMOD::Sound *sound;
   FMOD_MODE mode = threeDimensional ? FMOD_3D : FMOD_DEFAULT;

   switch (type) {
      case SoundType::Music:
         if (check(system->createStream(IOUtils::dataPath(fileName).c_str(), mode | FMOD_LOOP_NORMAL, nullptr, &sound))) {
            musicMap[fileName] = sound;
         }
         break;
      case SoundType::SoundEffect:
         if (check(system->createSound(IOUtils::dataPath(fileName).c_str(), mode, nullptr, &sound))) {
            effectsMap[fileName] = sound;
         }
         break;
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
