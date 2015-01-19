#ifndef RENDERER_H
#define RENDERER_H

#include "DebugRenderer.h"

#include <glm/glm.hpp>

class Context;
class Framebuffer;
class GameObject;
class Model;
class TextureMaterial;

class Renderer {
protected:
   /**
    * Renderer used to draw debug physics information
    */
   DebugRenderer debugRenderer;

   /**
    * Field of view of the perspective projection
    */
   float fov;

   /**
    * The perspective projection
    */
   glm::mat4 projectionMatrix;

   /**
    * If debug rendering is enabled
    */
   bool renderDebug;

   /**
    * Framebuffer used for composite rendering
    */
   UPtr<Framebuffer> framebuffer;

   /**
    * X/Y plane used for rendering the contents of the framebuffer to the back buffer
    */
   UPtr<Model> xyPlane;

   /**
    * Texture material for framebuffer color attachment
    */
   SPtr<TextureMaterial> colorTextureMaterial;

   /**
    * Texture material for framebuffer depth attachment
    */
   SPtr<TextureMaterial> depthTextureMaterial;

   /**
    * Loads the plane used render data from the framebuffer to the back buffer
    */
   void loadPlane();

   /**
    * Initializes the framebuffer and updates the attached texture materials
    */
   void initFramebuffer();

   /**
    * Renders the contents of the framebuffer onto the xy plane. The location / size of the rendered image are chosen based on the camera number / total number of cameras (for split-screen).
    */
   void renderFramebufferToPlane(int camera, int numCameras);

   /**
    * Renders the scene from the given camera's perspective
    */
   void renderFromCamera(Scene &scene, const GameObject &camera);

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
   void init(float fov, int width, int height);

   /**
    * Updates the renderer to match the new window resolution
    */
   void onWindowSizeChange(int width, int height);

   /**
    * Updates the renderer to match the properties of the new monitor
    */
   void onMonitorChange();

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
};

#endif
