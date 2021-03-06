#include "FancyAssert.h"
#include "Framebuffer.h"
#include "Texture.h"

Framebuffer::Framebuffer()
   : framebuffer(0), texture(nullptr), depthTexture(nullptr), width(0), height(0), initialized(false) {
   glGenFramebuffers(1, &framebuffer);
}

Framebuffer::~Framebuffer() {
   disable();

   glDeleteFramebuffers(1, &framebuffer);
}

bool Framebuffer::init(int width, int height) {
   ASSERT(width > 0 && height > 0, "Trying to use non-positive resolution for framebuffer");
   this->width = width;
   this->height = height;

   initialized = true;

   // Create the texture that will be the color attachment
   texture = std::make_shared<Texture>(GL_TEXTURE_2D);
   texture->bind();

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   texture->unbind();

   // Create the texture that will be the depth attachment
   depthTexture = std::make_shared<Texture>(GL_TEXTURE_2D);
   depthTexture->bind();

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
   depthTexture->unbind();

   // Attach the color / depth textures to the framebuffer
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id(), 0);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->id(), 0);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

bool Framebuffer::init() {
   // Determine the texture size from the viewport
   GLint viewport[4];
   glGetIntegerv(GL_VIEWPORT, viewport);

   return init(viewport[2], viewport[3]);
}

void Framebuffer::use() {
   ASSERT(initialized, "Trying to use uninitialized framebuffer");

   GLint viewport[4];
   glGetIntegerv(GL_VIEWPORT, viewport);
   lastViewport = Viewport(viewport[0], viewport[1], viewport[2], viewport[3]);

   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
   glViewport(0, 0, width, height);
}

void Framebuffer::disable() {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glViewport(lastViewport.x, lastViewport.y, lastViewport.width, lastViewport.height);
}

SPtr<Texture> Framebuffer::getTexture() const {
   ASSERT(initialized, "Trying to get texture from uninitialized framebuffer");
   return texture;
}

SPtr<Texture> Framebuffer::getDepthTexture() const {
   ASSERT(initialized, "Trying to get texture from uninitialized framebuffer");
   return depthTexture;
}
