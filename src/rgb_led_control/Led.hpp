#ifndef RGBLEDCONTROL_LED_H_
#define RGBLEDCONTROL_LED_H_

#include <Arduino.h>
#include <Wire.h>
#include "intensities.h"
#include "SpeedControl.hpp"
#include "PwmSource.h"

#ifdef PCA9685
#define MAX_INTENSITY 4095
#else
#define MAX_INTENSITY 255
#endif

const uint8_t kBitDimmable = 0x01;
const uint8_t kBitNewFactor = 0x02;
const uint8_t kBitNewMinPointerAtMax = 0x04;
const uint8_t kBitNewMaxPointerAtMin = 0x08;
const uint8_t kBitWaitAtMin = 0x10;
const uint8_t kBitWaitAtMax = 0x20;
const uint8_t kBitLedIsOn = 0x40;

const uint8_t pwmPins[] = {3, 5, 6, 9, 10, 11};

struct LedDefaultProperties {
  uint8_t default_booleans;
  uint8_t progmem_index;
  uint8_t factor;
  uint8_t offset;
  uint8_t pointer_min;
  uint8_t pointer_max;
};

class Led
{
private:
  SpeedControl speedControl;
  uint8_t _pointerMin = 0;
  uint8_t _pointerMax = 255;
  uint8_t _defaultPointerMin = 0;
  uint8_t _defaultPointerMax = 255;
  uint8_t _defaultFactor = 0xFF;
  bool _darker = true;
  bool _darkerHasChanged = false;
  bool _newMaxPointerAtMin = false;
  bool _newMinPointerAtMax = false;
  bool _newFactor = false;
  bool _waitAtMax = false;
  bool _waitAtMin = false;
  bool _pointerIsAtMax = false;
  bool _pointerIsAtMin = false;
  bool _pointerIsChangeable = true;
  bool _dimmable = true;
  bool _led_is_on = true;

protected:
  uint8_t _number = 0;
  uint8_t _progmemIndex = 0;
  uint8_t _pointer = 0;
  uint8_t _dimm_factor = 0xFF;
  uint8_t _color_factor = 0xFF;
  uint8_t _offset = 0;

public:
  ///////////////////////
  // get and set methods
  ///////////////////////

  /* default values*/
  uint8_t getDefaultPointerMin(void);
  void setDefaultPointerMin(uint8_t);
  uint8_t getDefaultPointerMax(void);
  void setGefaultPointerMin(uint8_t);
  uint8_t getDefaultFactor(void);
  void setDefaultFactor(void);

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


  /* control via pointer or factors */

  /** @return if controlled via pointer or not */
  bool getControlViaPointer(void);

  /** @param controlViaPointer */
  void setControlViaPointer(bool);

  /** @brief toggle controlViaPointer */
  void toggleControlViaPointer(void);


  /* offset */

  /** @return the offset of this LED */
  uint8_t getOffset(void);

  /** @param offset of this LED */
  void setOffset(uint8_t);


  /* progmem index */

  /** @return index of the table of intensities */
  uint8_t getProgmemIndex(void);

  /** @param intex of the table of intensitiies */
  void setProgmemIndex(uint8_t);


  /* Waiting */

  /** @return waitAtMax */
  bool getWaitAtMax(void);

  /** @param waitAtMax */
  void setWaitAtMax(bool waitAtMax);

  /** @return waitAtMin */
  bool getWaitAtMin(void);

  /** @param waitAtMax */
  void setWaitAtMin(bool waitAtMin);


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

  /** @return pointerIsChangeable */
  bool getPointerIsChangeable(void);

  /** @param pointerIsChangeable */
  void setPointerIsChangeable(bool);

  /** @return changePointer */
  bool getDimmable(void);

  /** @param changePointer */
  void setDimmable(bool);

  /** @return changePointer */
  bool getLedIsOn(void);

  /** @param changePointer */
  void setLedIsOn(bool);

  
  // factors
  
  /**@return factor*/
  uint8_t getFactor(void);

  /**@param factor*/
  void setFactor(uint8_t);

  /**@return colorFactor*/
  uint8_t getColorFactor(void);

  /**@param colorFactor*/
  void setColorFactor(uint8_t);

  /**@return factor*/
  bool getNewFactor(void);

  /**@return factor*/
  void setNewFactor(bool);
  
  /**@brief sets factor to default value*/
  void setFactor2Default(void);


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
  // methods dealing with the pointer to the intensities
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

  ///
  /// Save default properties to the internal eeprom
  ///
  void saveToEeprom(uint8_t);

  ///
  /// Load default properties from the internal eeprom
  ///
  void loadFromEeprom(uint8_t);

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

#endif    // RGB_LED_CONTROL_H_
