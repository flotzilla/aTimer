#include <Arduino.h>
#include "ButtonHandler.h"

using namespace std;
    
ButtonHandler::ButtonHandler(int pin, int delay, void (*OnActionCallBack)(), void (*OffActionCallBack)()){
    buttonPin = pin;
    delayVal = delay;

    OnAction = OnActionCallBack;
    OffAction = OffActionCallBack;
}

void ButtonHandler::Update(unsigned long currentMills){
    if ( ( currentMills - lastUpdateTime ) > delayVal ){
        lastUpdateTime = currentMills;

        int reading = digitalRead(buttonPin);

        if (reading != prevButtonState){
            lastDebounceTime = millis();
        }

        if ( (millis() - lastDebounceTime ) > debounceDelayVal ) {
            if (reading != buttonState) {
                buttonState = reading;    

                if (buttonState == LOW){
                    OnAction();
                }

                if (buttonState == HIGH){
                    OffAction();
                }
            }
        }

        prevButtonState = reading;
    }
}