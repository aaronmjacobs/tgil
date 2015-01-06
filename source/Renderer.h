#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

class Context;

class Renderer {
protected:
   float fov;
   glm::mat4 projectionMatrix;

public:
   Renderer();

   virtual ~Renderer();

   void prepare(float fov, int width, int height);

   void onWindowSizeChange(int width, int height);

   void render(const Context &context);
};

#endif
