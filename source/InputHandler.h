#ifndef INPUT_HANLDER_H
#define INPUT_HANLDER_H

#include "InputDevice.h"
#include "Types.h"

#include <vector>

class InputComponent;
class KeyMouseInputDevice;

class InputHandler {
protected:
   GLFWwindow* const window;
   std::vector<SPtr<InputDevice>> inputDevices;
   std::vector<InputValues> inputValues;
   SPtr<KeyMouseInputDevice> keyMouseInputDevice;
   InputValues keyMouseInputValues;

public:
   InputHandler(GLFWwindow* const window);

   virtual ~InputHandler();

   void pollInput();

   const InputValues& getInputValues(int device) const;

   const InputValues& getKeyMouseInputValues() const;

   int getNumDevices() const;

   double getMouseX() const;

   double getMouseY() const;

   bool isLeftMouseClicked() const;

   bool isRightMouseClicked() const;

   const std::vector<SPtr<InputDevice>>& getInputDevices() const;
};

#endif
