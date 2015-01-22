#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "Types.h"

class Context;
class Scene;

namespace SceneLoader {

SPtr<Scene> loadDefaultScene(const Context &context);

} // namespace SceneLoader

#endif
