///////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief interfaces eeprom content to play_of_lights
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _INTERFACE_EEPROM_H
#define _INTERFACE_EEPROM_H

#include <stdint.h>
#include "Led.h"

// Constant values describing P
const uint8_t kSizeOfGlobalProperties = 4:
const uint8_t kIndexOfNumberOfPlays = 0;
const uint8_t kIndexOfPlayAfterPor = 1;
const uint8_t kIndexOfLanguage = 2;
const uint8_t kIndexOfHexInput = 3;





class EepromContent {
  public:
  void ReadGlobalContent(void);
  void WriteGlobalContent(void);
  void ReadPlayOfLight(uint8_t);
  void WriteGlobalContent(uint8_t);

  private:
};


#endif //_INTERFACE_EEPROM_Huint8_
