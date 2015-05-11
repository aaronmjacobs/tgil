#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "Types.h"

#include <vector>

class Context;
class Scene;

namespace SceneLoader {

SPtr<Scene> loadMenuScene(const Context &context);

SPtr<Scene> loadWinScene(const Context &context);

SPtr<Scene> loadNextLevel(const Context &context);

} // namespace SceneLoader

#endif
