#ifndef RENDERER_H
#define RENDERER_H

class Renderer {
public:
   Renderer();
   virtual ~Renderer();

   void prepare();
   void render();
};

#endif
