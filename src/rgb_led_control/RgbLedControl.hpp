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

#define NUMBER_OF_LEDS 3
#define NUMBER_OF_PROGMEMS 10
#define DEFAULT_PROGMEM_NUMBER 0
#define DEFAULT_GLOBAL_FACTOR 0xFF
#define DELAY_TIME 20
#define DURATION_MAX 3
#define FULL_INTENSITY 255
#define DELAY_TEST 1000
#define ASCII_OFFSET 48
#define ADDRESS_LOOP_TIME 0
#define ADDRESS_PROGMEM_INDEX 1
#define ADDRESS_GLOBAL_FACTOR 2

class RgbLedControl
{
private:
  Led8bit led[NUMBER_OF_LEDS]; /**< Array of instances of the class Led */
  unsigned long cycleTime;
  unsigned long loopDuration;
  unsigned long oldMillis;
  bool outputOfLoopDuration;
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

  /**@brief This methods sets the beginning of an array.
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

  /**@brief This method toggles if the factor changes.*/
  void toggleNewFactor(void);

  /**@brief This mehtod toggles if the pointer chages.*/
  void toggleNewPointer(void);
  
  /**@brief This method reads the content of the EEPROM.
   *
   * The user can set the cycle time and the index of the
   * used PROGMEM. The setup function can read the stored
   * values. If a 255 is read, than the EEPROM has not been
   * programmed yet. Than the cycle time and the index are
   * set to default values.
   */
  void readEeprom();

  /**@brief This methods writes the current values to the EEPROM.
   *
   * If the user changed the loop time and the PROGMEM index,
   * he can save this to the EEPROM. So they will be loaded at
   * the next start.
   */
  void writeEeprom(void);

  /**@brief This method test all LEDs
   *
   * At lower intensities the decreases or increases can be
   * seen. With this method a new minimum value can be defined,
   * that it is more fluent.
   */
  void testAllLeds(void);

};

#endif
