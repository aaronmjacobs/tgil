#ifndef CONTEXT_H
#define CONTEXT_H

#include "Types.h"

class AssetManager;
class InputHandler;
class Scene;
struct GLFWwindow;

class Context {
private:
   static UPtr<Context> instance;

   Context(GLFWwindow* const window);

protected:
   const UPtr<AssetManager> assetManager;
   const UPtr<InputHandler> inputHandler;
   const SPtr<Scene> scene;

public:
   static void load(GLFWwindow* const window);
   static const Context& getInstance();

   virtual ~Context();

   void onWindowFocusGained() const;

   AssetManager& getAssetManager() const;
   InputHandler& getInputHandler() const;
   Scene& getScene() const;
};

#endif
