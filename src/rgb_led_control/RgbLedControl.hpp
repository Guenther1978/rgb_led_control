/**@file */

#ifndef RGB_LED_CONTROL_H
#define RGB_LED_CONTROL_H

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "Led.hpp"
#include "SpeedControl.hpp"
#include "PwmSource.h"

#ifdef PCA9685
#include <Adafruit_PWMServoDriver.h>
#endif

/* Constants for EEPROM  */
#define ADDRESS_NUMBER_OF_DEFAULT_PLAY 0
#define BYTES_NUMBER_DEFAULT_PLAY 1
#define MAX_NUMBER_OF_PLAYS 5
#define LENGTH_OF_LED_PROPERTIES 14
#define LENGTH_OF_PLAY_PROPERTIES 2 + (3 * LENGTH_OF_LED_PROPERTIES)
#define OFFSET_RED 2
#define OFFSET_GREEN OFFSET_RED + LENGTH_OF_LED_PROPERTIES
#define OFFSET_BLUE OFFSET_GREEN + LENGTH_OF_LED_PROPERTIES
#define OFFSET_LOOP_DURATION 0
#define OFFSET_GLOBAL_FACTOR 1
#define OFFSET_COLOR_PROGMEM_INDEX 1

/* Constants */
#define NUMBER_OF_LEDS 3
#define NUMBER_OF_PROGMEMS 6
#define LINEAR 5
#define NUMBER_OF_PLAYS 4
#define DEFAULT_PROGMEM_NUMBER 0
#define DEFAULT_PLAY_OF_LIGHT 0
#define DEFAULT_GLOBAL_FACTOR 0xFF
#define DELAY_TIME 20
#define DURATION_MAX 3
#define FULL_INTENSITY 255
#define DELAY_TEST 1000
#define ASCII_OFFSET 48

class RgbLedControl
{
private:
  #ifdef PCA9685
  Led16bit led[NUMBER_OF_LEDS];
  Adafruit_PWMServoDriver pwm;
  #else
  Led8bit led[NUMBER_OF_LEDS]; /**< Array of instances of the class Led */
  #endif
  unsigned char playOfLight;
  unsigned char numberOfPlays;
  unsigned long cycleTime;
  unsigned long loopDuration;
  unsigned long oldMillis;
  byte incomingByte;
public:
  /**@brief This function initializes the LightSnake class.
   *
   * The class contains one instance of the class
   * Adafruit_PWMServoDriver and an array of the
   * class Led. The random generator and the class
   * Serial will be initialized, too.
   */  
  void setup();
  
  /**@brief This function repeats the updates of
   * the intensites of the leds.
   *
   * Every element of the Led Class is called. Their
   * intensities are updated and at the min or max value
   * a new duration of the speed is evaluated. The new
   * intensities are send via the I2C bus to the PCA9685.
   */
  void loop();

  /** @return playOfLight */
  unsigned char getPlayOfLight(void);

  /**@brief sets the play of light*/
  void setPlayOfLight(bool);

  /**@brief This method prints an info
   *
   * The Serial Monitor of the Arduino IDE, PuTTY or picocom
   * can be used.
   */
  void help();

  /**@brief This methode prints the info of all Leds
   *
   * The number, intesity, darker and duration of
   * all LEDs are printed. And the time each the last
   * loop has needed and the cycle time of the loop.
   */
  void info();

  /**@return number
   *
   * This method is used by getting the number of a
   * LED and by getting the new duration.
   */
  int8_t getNumber();  

  /**@return boolean
   *
   * This method is used by getting a true or false.
   * The return value is an integer. 0 will be intepreted as false,
   * 1 as true. With a negative number as a return value, the calling
   * function will not update the boolean value, it stays true or false.
   */
  int8_t getBoolean(void);

  /**@brief This method sets a new duration time for the loop.
   *
   * The time sets the duration for the loop. If this value is
   * to small, the function will not wait and start immediately
   * with the next cycle.
   * After each cycle the methods waits till the duration of
   * a cycle is over. A loop duration can be set with a
   * char. This character represent a hexadecimal digit.
   * Hexadecimal number are used (0..9, A, B, C, D, E, F).
   * The new duration time is this digit multiplied by 5 ms.
   * At the end of the loop, the output of millis() is polled.
   * If the value 0 is choosen, the next loop starts immediately.
   */
  void changeLoopDuration(bool);

  /**@brief This method sets the beginning of an array.
   *
   * There can be different arrays containing the intensities.
   * With this method the index of the progmem is set globally
   * for all LEDs.
   */
  void setIndex(bool);

  /**@brief This methods sets the global factor.
   *
   * The global factor dims all LEDs.
   */
  void setGlobalFactor(bool);

  /**@brief This method sets the offset of the brightness of a LED.*/
  void setOffset(bool);

  /**@brief This method toggles if the factor changes.*/
  void toggleNewFactorAtMax(void);

  /**@brief This method toggles if the factor changes.*/
  void toggleNewFactorAtMin(void);

  /**@brief This mehtod toggles if the pointer chages.*/
  void toggleNewPointerAtMax(void);

  /**@brief This mehtod toggles if the pointer chages at min. position.*/
  void toggleNewMinPointerAtMax(void);

  /**@brief This mehtod toggles if the pointer chages at max. position.*/
  void toggleNewMaxPointerAtMin(void);

  /**@brief This method toggles in waiting or not at the max. pointer */
  void toggleWaitAtMax1(void);

  /**@brief This method toggles in waiting or not at the min. pointer */
  void toggleWaitAtMin1(void);

  /**@brief This method toggles in staying or not at the max. pointer */
  void toggleWaitAtMax2(void);

  /**@brief This method toggles in staying or not at the min. pointer */
  void toggleWaitAtMin2(void);

  /**@brief This methode counts the LEDs which are getting darker */
  uint8_t countLedsGettingDarker(void);

  /**@brief Sets the colorfactors */
  void setColorFactors(void);

  /**@brief This method calls probertiesOfLed with index of the blue LED */
  void blueLED(void);

  /**@brief This method calls probertiesOfLed with index of the green LED */
  void greenLED(void);

  /**@brief This method calls probertiesOfLed with index of the red LED */
  void redLED(void);

  /**@brief This method initilizes the properties of the LED */
  void propertiesOfLed(uint8_t);

  /**@brief Reads the play of light at start from the EEPROM. */
  void readEeprom(void);

  /**@param playOfLight
   *
   * The properties of the LEDs are set regarding to the
   * playOfLight number.
   */
  void readEeprom(uint8_t);

  /**@brief Writes the new playOfLight for the start to the EEPROM */
  void writeEeprom(void);

  /**@brief This methods writes the current values to the EEPROM.
   *
   * If the changes of the properties of the LEDs should be saved,
   * than this mehtod should be called with the
   */
  void writeEeprom(uint8_t);

  /**@brief This method test all LEDs
   *
   * At lower intensities the decreases or increases can be
   * seen. With this method a new minimum value can be defined,
   * that it is more fluent.
   */
  void testAllLeds(void);

};

#endif
