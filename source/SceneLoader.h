#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "Types.h"

class Context;
class Scene;

namespace SceneLoader {

SPtr<Scene> loadBasicScene(const Context &context);

SPtr<Scene> loadTowerScene(const Context &context);

} // namespace SceneLoader

#endif
