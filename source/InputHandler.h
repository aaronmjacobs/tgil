#ifndef INPUT_HANLDER_H
#define INPUT_HANLDER_H

#include "InputMap.h"
#include "Types.h"

#include <vector>

class InputHandler {
protected:
   GLFWwindow* const window;
   // TODO Handle multiple input sources (which can be connected / disconnected)
   SPtr<InputMap> inputMap;
   std::vector<InputValues> inputValues;

public:
   InputHandler(GLFWwindow* const window);

   virtual ~InputHandler();

   void pollInput();

   const InputValues& getInputValues(int player) const;
};

#endif
