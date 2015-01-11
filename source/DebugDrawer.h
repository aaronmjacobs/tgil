#ifndef DEBUG_DRAWER_H
#define DEBUG_DRAWER_H

#include <bullet/LinearMath/btIDebugDraw.h>
#include <glm/glm.hpp>

#include <vector>

class ShaderProgram;

struct Line {
   glm::vec3 from, to;

   Line(glm::vec3 from, glm::vec3 to)
      : from(from), to(to) {}
};

typedef Line LineColor;

class DebugDrawer : public btIDebugDraw {
protected:
   int debugMode;
   std::vector<Line> lines;
   std::vector<LineColor> colors;

public:
   DebugDrawer();

   virtual ~DebugDrawer();

   const std::vector<Line>& getLines() const {
      return lines;
   }

   const std::vector<LineColor>& getColors() const {
      return colors;
   }

   void clear();

   virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);

   virtual void drawLine(const btVector3& from,const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

   virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) {} // TODO Implement

   virtual void reportErrorWarning(const char *warningString);

   virtual void draw3dText(const btVector3 &location, const char *textString) {} // TODO Implement

   virtual void setDebugMode(int debugMode) {
      this->debugMode = debugMode;
   }

   virtual int getDebugMode () const {
      return debugMode;
   }
};

#endif
