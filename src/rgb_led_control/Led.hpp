#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "intensities.h"
#include "SpeedControl.hpp"

class Led
{
private:
  SpeedControl speedControl;
  unsigned char _color = ' ';
  uint8_t _number = 0;
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
  bool _pointerIsAtMax = false;
  bool _pointerIsAtMin = false;

protected:
  uint8_t _pointer = 0;
  uint8_t _factor = 0xFF;
  uint8_t _globalFactor = 0xFF;
  uint8_t _colorFactor = 0xFF;

public:
  ///////////////////////
  // get ans set methods
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
  uint8_t _intensity = 0xFF;
  uint8_t _progmemIndex = 0;

public:
  /** @return intensity */
  uint8_t getIntensity(void);

  /** @param intensity */
  void setIntensity(uint8_t);

  /** @return index of the table of intensities */
  uint8_t getProgmemIndex(void);

  /** @param intex of the table of intensitiies */
  void setProgmemIndex(uint8_t);

  /**@return the content of the PROGMEM array.
   *
   * The array with the intensities is included in the header file
   * 'intensities.h'. This file has been created by the python
   * script 'progmen_creator.py'.
   */
  void pointer2int(void);
};

#endif
