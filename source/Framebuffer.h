#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "GLIncludes.h"
#include "Types.h"
#include "Viewport.h"

class Texture;

class Framebuffer {
protected:
   /**
    * Framebuffer object
    */
   GLuint framebuffer;

   /**
    * Color attachment texture
    */
   SPtr<Texture> texture;

   /**
    * Depth attachment texture
    */
   SPtr<Texture> depthTexture;

   /**
    * Resolution of the textures
    */
   int width;
   int height;

   /**
    * If the framebuffer has been initialized
    */
   bool initialized;

   /**
    * The last viewport used before this framebuffer was used
    */
   Viewport lastViewport;

public:
   Framebuffer();

   virtual ~Framebuffer();

   /**
    * Initializes the textures to the given resolution; may be called multiple times
    */
   bool init(int width, int height);

   /**
    * Initializes the textures to the resolution of the viewport; may be called multiple times
    */
   bool init();

   /**
    * Causes all drawing to occur in the framebuffer
    */
   void use();

   /**
    * Causes all drawing to occur in the back buffer
    */
   void disable();

   /**
    * Returns the texture of the color attachment
    */
   SPtr<Texture> getTexture() const;

   /**
    * Returns the texture of the depth attachment
    */
   SPtr<Texture> getDepthTexture() const;
};

#endif
