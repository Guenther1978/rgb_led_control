#include "Button.hpp"

void Button::init(void)
  {
    pinMode(_pin, INPUT);
  }

uint8_t Button::getPin(void)
  {
    return _pin;
  }

void Button::setPin(uint8_t pin)
  {
    _pin = pin;
  }

bool Button::getIsPressed(void)
  {
    isPressed = digitalRead(_pin);
    return isPressed;
  }

uint8_t Button::getDurationOfPressing(void)
  {
    unsigned long startTime = 0;
    unsigned long stopTime = 0;

    if(getIsPressed())
      {
        startTime = millis();
        while(getIsPressed());
        stopTime = millis();
      }

    durationOfPressing = (stopTime - startTime) / 1000;

    return durationOfPressing;
  }
