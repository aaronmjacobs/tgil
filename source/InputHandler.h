#ifndef INPUT_HANLDER_H
#define INPUT_HANLDER_H

#include "InputDevice.h"
#include "Types.h"

#include <vector>

class InputComponent;

class InputHandler {
protected:
   GLFWwindow* const window;
   std::vector<SPtr<InputDevice>> inputDevices;
   std::vector<InputValues> inputValues;
   std::vector<WPtr<InputComponent>> inputComponents;

public:
   InputHandler(GLFWwindow* const window);

   virtual ~InputHandler();

   void pollInput();

   void registerInputComponent(SPtr<InputComponent> inputComponent);

   const InputValues& getInputValues(int player) const;
};

#endif
