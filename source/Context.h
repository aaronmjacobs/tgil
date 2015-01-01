#ifndef CONTEXT_H
#define CONTEXT_H

#include "Types.h"

class AssetManager;
class InputHandler;
class Scene;
struct GLFWwindow;

class Context {
protected:
   const UPtr<AssetManager> assetManager;
   const UPtr<InputHandler> inputHandler;
   const UPtr<Scene> scene;

public:
   Context(GLFWwindow* const window);

   virtual ~Context();

   AssetManager& getAssetManager() const;
   InputHandler& getInputHandler() const;
   Scene& getScene() const;
};

#endif
