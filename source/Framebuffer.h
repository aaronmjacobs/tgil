#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "GLIncludes.h"

class Framebuffer {
protected:
   /**
    * Framebuffer object
    */
   GLuint framebuffer;

   /**
    * Color attachment texture
    */
   GLuint texture;

   /**
    * Depth attachment texture
    */
   GLuint depthTexture;

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
    * Deletes the attachment textures
    */
   void deleteTextures();

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
   void use() const;

   /**
    * Causes all drawing to occur in the back buffer
    */
   void disable() const;

   /**
    * Returns the texture ID of the color attachment
    */
   GLuint getTextureID() const;

   /**
    * Returns the texture ID of the depth attachment
    */
   GLuint getDepthTextureID() const;
};

#endif
