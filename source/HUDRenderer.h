#ifndef HUD_RENDERER_H
#define HUD_RENDERER_H

#include "GLIncludes.h"
#include "Material.h"
#include "Types.h"

#include <glm/glm.hpp>
#include <vector>

class Model;
class PlayerLogicComponent;
class TextureMaterial;

class HUDElement {
public:
   // ID of the texture to draw on the HUD
   GLuint textureID;

   // Position on screen, as a percentage ((0,0) = lower left, (1,1) = upper right)
   glm::vec2 position;

   // Scale on screen (1 = matches height of screen)
   glm::vec2 scale;

   // Amount of image filled from lower left ((0,0) = gone, (1,1) = full image)
   glm::vec2 fill;

   // Color to tint the texture
   glm::vec3 tint;

   // Opacity of the texture
   float opacity;

   std::function<void(HUDElement &element, const PlayerLogicComponent &playerLogic)> updateLogic;

   HUDElement(GLuint textureID, glm::vec2 position, glm::vec2 scale);

   virtual ~HUDElement();

   void setUpdateLogic(std::function<void(HUDElement &element, const PlayerLogicComponent &playerLogic)> updateLogic) {
      this->updateLogic = updateLogic;
   }

   void update(const PlayerLogicComponent &playerLogic);
};

class HUDRenderer {
protected:
   UPtr<Model> xyPlane;

   SPtr<TextureMaterial> textureMaterial;

   GLint uTransform;
   GLint uOpacity;
   GLint uTint;
   GLint uFill;

   std::vector<HUDElement> elements;

   void loadPlane();

   void loadElements();

public:
   HUDRenderer();

   virtual ~HUDRenderer();

   void init();

   void attach(const HUDElement &element);

   void render(const PlayerLogicComponent &playerLogic, int width, int height);
};

#endif
