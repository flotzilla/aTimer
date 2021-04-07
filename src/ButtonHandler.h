#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>

class ButtonHandler{
    
public:
    int buttonPin;
    unsigned int delayVal = 30;
    
    int prevButtonState = HIGH;
    int buttonState;
    unsigned long lastUpdateTime;
    unsigned long prevTime;

    unsigned long lastDebounceTime;
    unsigned long prevDebounceTime;
    unsigned int debounceDelayVal = 10;

    void (*OnAction)();
    void (*OffAction)();

    ButtonHandler(int pin, int delay, void (*OnActionCallBack)(), void (*OffActionCallBack)());

    void Update(unsigned long currentMills);
};


#endif