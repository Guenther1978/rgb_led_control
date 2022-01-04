#ifndef SPEED_CONTROL_H
#define SPEED_CONTROL_H

#include <Arduino.h>

class SpeedControl
{
private:
  uint8_t _number = 0;
  uint8_t _duration = 1;
  uint8_t _max_duration = 5;
  uint8_t _counter = 0;
public:
  /**@return counter*/
  uint8_t getCounter();

  /**@param counter of the LED*/
  void setCounter(uint8_t);

  /**@return number of the LED*/
  uint8_t getNumber();

  /**@param number of the LED*/
  void setNumber(uint8_t);

  /**@return duration of the entensity*/
  uint8_t getDuration();

  /**@param duration of the intensity
   *
   * The duration specifies the time at an intensity
   */
  void setDuration(uint8_t);

  /**
   * @return (counter == 0)
   *
   * The method decreases the counter, if the counter
   * has the value zero, true is returned.
   */
  bool count();
};

#endif
