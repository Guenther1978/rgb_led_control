/** @file */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button
{
  private:
  uint8_t _pin = 2;
  bool isPressed;
  uint8_t durationOfPressing;

  public:
  /**@brief initialization*/
  void init(void);

  /**@return pin*/
  uint8_t getPin(void);

  /**@param pin*/
  void setPin(uint8_t);

  /**@return isPressed*/
  bool getIsPressed(void);

  /**@return durationOfPresseing*/
  uint8_t getDurationOfPressing(void);
};

#endif
