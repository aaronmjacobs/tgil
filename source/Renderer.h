#ifndef RENDERER_H
#define RENDERER_H

class Context;

class Renderer {
public:
   Renderer();
   virtual ~Renderer();

   void prepare();
   void render(const Context &context);
};

#endif
