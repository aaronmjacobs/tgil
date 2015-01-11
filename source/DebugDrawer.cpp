#include "Conversions.h"
#include "DebugDrawer.h"
#include "LogHelper.h"

DebugDrawer::DebugDrawer()
   : debugMode(DBG_DrawWireframe) {
}

DebugDrawer::~DebugDrawer() {
}

void DebugDrawer::clear() {
   lines.clear();
   colors.clear();
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
   drawLine(from, to, color, color);
}

void DebugDrawer::drawLine(const btVector3& from,const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
   lines.push_back(Line(toGlm(from), toGlm(to)));
   colors.push_back(LineColor(toGlm(fromColor), toGlm(toColor)));
}

void DebugDrawer::reportErrorWarning(const char *warningString) {
   LOG_WARNING("DebugDrawer: " << warningString);
}
