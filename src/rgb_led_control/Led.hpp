#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <Wire.h>
#include "intensities.h"
#include "SpeedControl.hpp"
#include "PwmSource.h"

const uint8_t pwmPins[] = {3, 5, 6, 9, 10, 11};

class Led
{
private:
  SpeedControl speedControl;
  unsigned char _color = ' ';
  uint8_t _pointerMin = 0;
  uint8_t _pointerMax = 255;
  uint8_t _defaultPointerMin = 0;
  uint8_t _defaultPointerMax = 255;
  uint8_t _defaultFactor = 0xFF;
  bool _darker = true;
  bool _darkerHasChanged = false;
  bool _newMaxPointerAtMin = false;
  bool _newMinPointerAtMax = false;
  bool _newFactorAtMax = false;
  bool _newFactorAtMin = false;
  bool _waitAtMax1 = false;
  bool _waitAtMin1 = false;
  bool _waitAtMax2 = false;
  bool _waitAtMin2 = false;
  bool _pointerIsAtMax = false;
  bool _pointerIsAtMin = false;
  bool _pointerIsChangeable = true;

protected:
  uint8_t _number = 0;
  uint8_t _progmemIndex = 0;
  uint8_t _pointer = 0;
  uint8_t _factor = 0xFF;
  uint8_t _globalFactor = 0xFF;
  uint8_t _colorFactor = 0xFF;

public:
  ///////////////////////
  // get and set methods
  ///////////////////////

  /* number */

  /** @return number of this LED */
  uint8_t getNumber(void);

  /** @param number of this LED */
  void setNumber(uint8_t);


  /* color */

  /** @return color of this LED */
  unsigned char getColor(void);

  /** @param color of this LED */
  void setColor(unsigned char);

  /** @return index of the table of intensities */
  uint8_t getProgmemIndex(void);

  /** @param intex of the table of intensitiies */
  void setProgmemIndex(uint8_t);


  /* Waiting */

  /** @return waitAtMax */
  bool getWaitAtMax1(void);

  /** @param waitAtMax */
  void setWaitAtMax1(bool waitAtMax);

  /** @brief toggle waitAtMax */
  void toggleWaitAtMax1(void);

  /** @return waitAtMin */
  bool getWaitAtMin1(void);

  /** @param waitAtMax */
  void setWaitAtMin1(bool waitAtMin);

  /** @brief toggle waitAtMin */
  void toggleWaitAtMin1(void);

  /** @return waitAtMax */
  bool getWaitAtMax2(void);

  /** @param waitAtMax */
  void setWaitAtMax2(bool waitAtMax);

  /** @brief toggle waitAtMax */
  void toggleWaitAtMax2(void);

  /** @return waitAtMin */
  bool getWaitAtMin2(void);

  /** @param waitAtMax */
  void setWaitAtMin2(bool waitAtMin);

  /** @brief toggle waitAtMin */
  void toggleWaitAtMin2(void);

  /* pointer */
  
  /** @return pointer to the intensities */
  uint8_t getPointer(void);

  /** @param pointer to the intensities */
  void setPointer(uint8_t);

  /** @return minimal value of this pointer */
  uint8_t getMinPointer(void);

  /** @param minimal value of this pointer */
  void setMinPointer(uint8_t);

  /** @return new minumum value of this pointer */
  bool getNewMinPointerAtMax(void);

  /** @param new minimum value of this pointer */
  void setNewMinPointerAtMax(bool);

  /** @brief toggle if a new minmum value of the pointer is set or not */
  void toggleNewMinPointerAtMax(void);

  /** @brief sets the maximum Pointer to the default value */
  void setMinPointer2Default(void);

  /** @return maximum value of this pointer */
  uint8_t getMaxPointer(void);

  /** @param maximum value of this pointer */
  void setMaxPointer(uint8_t);

  /** @return new maximum value of this pointer */
  bool getNewMaxPointerAtMin(void);

  /** @param new maximum value of this pointer */
  void setNewMaxPointerAtMin(bool);

  /** @brief toggle if a new maximum value of the pointer is set or not */
  void toggleNewMaxPointerAtMin(void);

  /** @brief sets the maximum Pointer to the default value */
  void setMaxPointer2Default(void);

  /** @return changePointer */
  bool getPointerIsChangeable(void);

  /** @param changePointer */
  void setPointerIsChangeable(bool);

  
  // factors
  
  /**@return factor*/
  uint8_t getFactor(void);

  /**@param factor*/
  void setFactor(uint8_t);

  /**@return factor*/
  uint8_t getGlobalFactor(void);

  /**@param factor*/
  void setGlobalFactor(uint8_t);

  /**@return factor*/
  bool getNewFactorAtMax(void);

  /**@return factor*/
  void setNewFactorAtMax(bool);
  
  /**@return factor*/
  bool getNewFactorAtMin(void);

  /**@return factor*/
  void setNewFactorAtMin(bool);

  /**@brief sets factor to default value*/
  void setFactor2Default(void);

  /*@brief toggle if new factors are set or not*/
  void toggleNewFactorAtMin(void);
  void toggleNewFactorAtMax(void);
  
  
  // _darker

  /**@return darker of the LED*/
  bool getDarker(void);

  /**@param darker if the brightness of the LED decreases*/
  void setDarker(bool);

  /**@brief inverts darker*/
  void invertDarker(void);


  // _darkerHasChanged, _pointerAtMax, pointerAtMin
  
  /**@return _darkerHasChanged*/
  bool getDarkerHasChanged(void);

  /**@return _intensityAtMin*/
  bool getPointerIsAtMin(void);

  /**@return _intensityAtMax*/
  bool getPointerIsAtMax(void);
  

  ///////////////////////////////////////////////////////
  // methods dealing with the pointer to thw intensities
  ///////////////////////////////////////////////////////
  
  /**Increases the pointer to the intensities.
   *
   * If the new value of the pointer is equal
   * to the size of the array,
   * the pointer will be decreased in the next step;
   */
  void increasePointer(void);

  /**Decreases the pointer to the intensities.
  *
  * If the new value of the pointer is equal
  * to zero, the pointer will be increased in the next step.
  */
  void decreasePointer(void);

  /**Increases or decreases the pointer.
   *
   * In dependency of the boolean value of darker the method
   * increasePointer or decreasePointer is called.
   */
  void changePointer(void);


  ////////////////////////////////////////////////
  // methods dealing with the class SpeedControl
  ////////////////////////////////////////////////

  /**@brief Call counter method of class SpeedControl
   *
   * The property counter of the aggregated class SpeedControl
   * is decreased. If its value is equal to zero, true is returned.
   * The variable is the initialized with the duration. 
   */
  bool letSpeedControlCount(void);

  /**@param duration time at one intensity (property of the class SpeedControl)*/
  void setSpeedControlDuration(uint8_t);

  /**@return duration (the time at one intensity, property of the class SpeedControl)*/
  uint8_t getSpeedControlDuration(void);
  
  /**@param counter
   *
   * If a longer duration is wanted, the counter can be set to
   * a value greater than duration.
   */
  void setSpeedControlCounter(uint8_t);

  /**@return counter counts from duration to zero*/
  uint8_t getSpeedControlCounter(void);
};

class Led8bit : public Led
{
private:
  uint8_t _pin;
  uint8_t _intensity = 0xFF;

public:
  /** @return pin */
  uint8_t getPin(void);

  /** @param pin */
  void setPin(uint8_t);

  /** @brief set pin to one of the pwm outputs */
  void setPin2default(void);

  /** @return intensity */
  uint8_t getIntensity(void);

  /** @param intensity */
  void setIntensity(uint8_t);

  /** @brief copy the content of the PROGMEM array to the intensity
   *
   * The array with the intensities is included in the header file
   * 'intensities.h'. This file has been created by the python
   * script 'progmen_creator.py'.
   */
  void pointer2int(void);

  /** @brief send the intensity to the output periphic */
  void int2output(void);
};

class Led16bit : public Led
{
private:
  uint16_t _intensity = 0x0FFF;

public:
  /** @return intensity */
  uint16_t getIntensity(void);

  /** @param intensity */
  void setIntensity(uint16_t);

  /** @brief copy the content of the PROGMEM array to the intensity
   *
   * The array with the intensities is included in the header file
   * 'intensities.h'. This file has been created by the python
   * script 'progmen_creator.py'.
   */
  void pointer2int(void);

  /** @brief send the intensity to the output periphic */
  void int2output(void);
};

#endif
