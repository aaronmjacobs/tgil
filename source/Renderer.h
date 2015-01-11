#ifndef RENDERER_H
#define RENDERER_H

#include "DebugRenderer.h"

#include <glm/glm.hpp>

class Context;

class Renderer {
protected:
   DebugRenderer debugRenderer;
   float fov;
   glm::mat4 projectionMatrix;
   bool renderDebug;

public:
   Renderer();

   virtual ~Renderer();

   void init(float fov, int width, int height);

   void onWindowSizeChange(int width, int height);

   void onMonitorChange();

   void render(Scene &scene);

   void enableDebugRendering(bool enabled) {
      renderDebug = enabled;
   }

   bool debugRenderingEnabled() const {
      return renderDebug;
   }
};

#endif
