#ifndef RENDERER_H
#define RENDERER_H

#include "DebugRenderer.h"
#include "HUDRenderer.h"
#include "PostProcessRenderer.h"
#include "SkyRenderer.h"
#include "TextRenderer.h"

#include <glm/glm.hpp>

class GameObject;
class ShadowMap;
class ShadowMapManager;

struct Viewport {
   int x;
   int y;
   int width;
   int height;

   Viewport(int x, int y, int width, int height)
   : x(x), y(y), width(width), height(height) {
   }
};

class Renderer {
protected:
   /**
    * Renderer used to draw the HUD for each player
    */
   HUDRenderer hudRenderer;

   /**
    * Renderer used to draw post-processing effects
    */
   PostProcessRenderer postProcessRenderer;

   /**
    * Renderer used to draw the sky
    */
   SkyRenderer skyRenderer;

   /**
    * Renderer used to draw text
    */
   TextRenderer textRenderer;

   /**
    * Renderer used to draw debug physics information
    */
   DebugRenderer debugRenderer;

   /**
    * Width of the framebuffer (in pixels)
    */
   int width;

   /**
    * Height of the framebuffer (in pixels)
    */
   int height;

   /**
    * Width of the window (in screen coordinates)
    */
   int windowWidth;

   /**
    * Height of the window (in screen coordinates)
    */
   int windowHeight;

   /**
    * The pixel density of the framebuffer
    */
   float pixelDensity;

   /**
    * Field of view of the perspective projection
    */
   float fov;

   /**
    * Number of frames that need clear calls
    */
   int clearFramesNeeded;

   /**
    * The perspective projection
    */
   glm::mat4 projectionMatrix;

   UPtr<ShadowMapManager> shadowMapManager;

   /**
    * If debug rendering is enabled
    */
   bool renderDebug;

   void updatePixelDensity();

   void renderShadowMaps(Scene &scene);

   void prepareLights(Scene &scene);

   void renderShadowMap(Scene &scene, SPtr<GameObject> light);

   void renderShadowMapFace(Scene &scene, SPtr<GameObject> light, SPtr<ShaderProgram> shadowProgram, int face = -1);

   /**
    * Renders the scene from the given camera's perspective
    */
   void renderFromCamera(Scene &scene, const GameObject &camera, const Viewport &viewport);

   /**
    * Renders full-screen postprocessing effects
    */
   void renderFullscreenPost(Scene &scene);

   /**
    * Renders the debug physics information for the scene
    */
   void renderDebugInfo(Scene &scene, const glm::mat4 &viewMatrix);

public:
   Renderer();

   virtual ~Renderer();

   /**
    * Initializes the renderer, setting OpenGL properties and preparing rendering subsystems
    */
   void init(float fov, int width, int height, int windowWidth, int windowHeight);

   /**
    * Updates the renderer to match the new framebuffer resolution
    */
   void onFramebufferSizeChange(int width, int height);

   /**
    * Updates the renderer to match the new window resolution
    */
   void onWindowSizeChange(int width, int height);

   /**
    * Renders the given scene
    */
   void render(Scene &scene);

   /**
    * Enables / disables rendering of debug physics information
    */
   void enableDebugRendering(bool enabled) {
      renderDebug = enabled;
   }

   /**
    * Returns whether debug physics rendering is enabled
    */
   bool debugRenderingEnabled() const {
      return renderDebug;
   }

   /**
    * Gets the current projection matrix
    */
   const glm::mat4& getProjectionMatrix() const {
      return projectionMatrix;
   }

   int getWindowWidth() const {
      return windowWidth;
   }

   int getWindowHeight() const {
      return windowHeight;
   }

   float getPixelDensity() const {
      return pixelDensity;
   }
};

#endif
