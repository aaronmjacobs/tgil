#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "Types.h"

class Context;
class Scene;

namespace SceneLoader {

SPtr<Scene> loadTowerScene(const Context &context);

SPtr<Scene> loadCenterPlatformScene(const Context &context);

SPtr<Scene> loadMiniTowersScene(const Context &context);

SPtr<Scene> loadSparsePlatformScene(const Context &context);

SPtr<Scene> loadSpikyScene(const Context &context);

SPtr<Scene> loadFourTowersScene(const Context &context);

SPtr<Scene> loadNextScene(const Context &context);

} // namespace SceneLoader

#endif
