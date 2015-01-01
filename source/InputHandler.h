#ifndef INPUT_HANLDER_H
#define INPUT_HANLDER_H

#include "InputMap.h"
#include "Types.h"

class InputHandler {
protected:
   GLFWwindow* const window;
   // TODO Handle multiple input sources (which can be connected / disconnected)
   SPtr<InputMap> inputMap;

public:
   InputHandler(GLFWwindow* const window);

   virtual ~InputHandler();

   const InputValues& getInputValues(int player) const;
};

#endif
