#ifndef PLAYER_GRAPHICS_COMPONENT_H
#define PLAYER_GRAPHICS_COMPONENT_H

#include "GraphicsComponent.h"

class PlayerGraphicsComponent : public GraphicsComponent {
protected:
   SPtr<Model> headModel;
   SPtr<Model> handModel;
   SPtr<Model> footModel;
   glm::vec3 headOffset;
   glm::vec3 leftHandOffset;
   glm::vec3 rightHandOffset;
   glm::vec3 leftFootOffset;
   glm::vec3 rightFootOffset;

   void drawAppendages(const RenderData &renderData, const glm::mat4 &rotMatrix, const glm::mat4 &scaleMatrix);

   void drawAppendage(const RenderData &renderData, const glm::mat4 &rotMatrix, const glm::mat4 &vertRotMatrix, const glm::mat4 &scaleMatrix, SPtr<Model> model, const glm::vec3 &offset);

public:
   PlayerGraphicsComponent(GameObject &gameObject);

   virtual ~PlayerGraphicsComponent();

   virtual void draw(const RenderData &renderData);

   void setHeadModel(SPtr<Model> headModel) {
      this->headModel = headModel;
   }

   void setHandModel(SPtr<Model> handModel) {
      this->handModel = handModel;
   }

   void setFootModel(SPtr<Model> footModel) {
      this->footModel = footModel;
   }

   const glm::vec3& getHeadOffset() const {
      return headOffset;
   }

   const glm::vec3& getLeftHandOffset() const {
      return leftHandOffset;
   }

   const glm::vec3& getRightHandOffset() const {
      return rightHandOffset;
   }

   const glm::vec3& getLeftFootOffset() const {
      return leftFootOffset;
   }

   const glm::vec3& getRightFootOffset() const {
      return rightFootOffset;
   }

   void setHeadOffset(const glm::vec3 &offset) {
      headOffset = offset;
   }

   void setLeftHandOffset(const glm::vec3 &offset) {
      leftHandOffset = offset;
   }

   void setRightHandOffset(const glm::vec3 &offset) {
      rightHandOffset = offset;
   }

   void setLeftFootOffset(const glm::vec3 &offset) {
      leftFootOffset = offset;
   }

   void setRightFootOffset(const glm::vec3 &offset) {
      rightFootOffset = offset;
   }
};


#endif
