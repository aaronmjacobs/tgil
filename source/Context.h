#ifndef CONTEXT_H
#define CONTEXT_H

#include "Types.h"

class AssetManager;
class InputHandler;
class Renderer;
class Scene;
struct GLFWwindow;
struct InputValues;

class Context {
private:
   static UPtr<Context> instance;

   Context(GLFWwindow* const window);

protected:
   GLFWwindow* const window;
   const UPtr<AssetManager> assetManager;
   const UPtr<InputHandler> inputHandler;
   const UPtr<Renderer> renderer;
   SPtr<Scene> scene;

   void handleSpecialInputs(const InputValues &inputValues) const;

public:
   static void load(GLFWwindow* const window);
   static const Context& getInstance();

   virtual ~Context();

   void init();

   void tick(const float dt) const;

   void onWindowFocusGained() const;

   AssetManager& getAssetManager() const;
   InputHandler& getInputHandler() const;
   Renderer& getRenderer() const;
   Scene& getScene() const;
};

#endif
