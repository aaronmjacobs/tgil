#ifndef VIEWPORT_H
#define VIEWPORT_H

struct Viewport {
   int x;
   int y;
   int width;
   int height;

   Viewport()
      : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {
   }

   Viewport(int x, int y, int width, int height)
      : x(x), y(y), width(width), height(height) {
   }

   Viewport& operator=(const Viewport &other) {
      x = other.x;
      y = other.y;
      width = other.width;
      height = other.height;
      return *this;
   }
};

#endif
