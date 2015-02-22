#ifndef CONTEXT_H
#define CONTEXT_H

#include "Types.h"

class AssetManager;
class InputHandler;
class Renderer;
class Scene;
class TextureUnitManager;
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
   const UPtr<TextureUnitManager> textureUnitManager;
   SPtr<Scene> scene;
   float runningTime;
   float timeSinceWinner;

   void handleSpecialInputs(const InputValues &inputValues) const;

   void checkForWinner();

public:
   static void load(GLFWwindow* const window);
   static Context& getInstance();

   virtual ~Context();

   void init();

   void tick(const float dt);

   void onWindowFocusGained() const;

   AssetManager& getAssetManager() const;
   InputHandler& getInputHandler() const;
   Renderer& getRenderer() const;
   Scene& getScene() const;
   TextureUnitManager& getTextureUnitManager() const;

   void setScene(SPtr<Scene> scene);

   float getRunningTime() const {
      return runningTime;
   }
};

#endif
