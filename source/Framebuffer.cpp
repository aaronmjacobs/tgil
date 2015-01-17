#include "FancyAssert.h"
#include "Framebuffer.h"

Framebuffer::Framebuffer()
   : framebuffer(0), texture(0), depthTexture(0), width(0), height(0), initialized(false) {
   glGenFramebuffers(1, &framebuffer);
}

Framebuffer::~Framebuffer() {
   disable();

   if (initialized) {
      deleteTextures();
   }

   glDeleteFramebuffers(1, &framebuffer);
}

void Framebuffer::deleteTextures() {
   glDeleteTextures(1, &texture);
   glDeleteTextures(1, &depthTexture);
}

bool Framebuffer::init(int width, int height) {
   ASSERT(width > 0 && height > 0, "Trying to use non-positive resolution for framebuffer");
   this->width = width;
   this->height = height;

   // Delete any pre-existing textures
   if (initialized) {
      deleteTextures();
   }
   initialized = true;

   // Create the texture that will be the color attachment
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glBindTexture(GL_TEXTURE_2D, 0);

   // Create the texture that will be the depth attachment
   glGenTextures(1, &depthTexture);
   glBindTexture(GL_TEXTURE_2D, depthTexture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
   glBindTexture(GL_TEXTURE_2D, 0);

   // Attach the color / depth textures to the framebuffer
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

bool Framebuffer::init() {
   // Determine the texture size from the viewport
   GLint viewport[4];
   glGetIntegerv(GL_VIEWPORT, viewport);

   return init(viewport[2], viewport[3]);
}

void Framebuffer::use() const {
   ASSERT(initialized, "Trying to use uninitialized framebuffer");
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::disable() const {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
