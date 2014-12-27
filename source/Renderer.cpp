#include "GLIncludes.h"
#include "Renderer.h"

Renderer::Renderer() {
}

Renderer::~Renderer() {
}

void Renderer::prepare() {
   glClearColor(0.15f, 0.6f, 0.4f, 1.0f);

   // Depth Buffer Setup
   glClearDepth(1.0f);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   // Back face culling
   glCullFace(GL_BACK);
}

void Renderer::render() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
