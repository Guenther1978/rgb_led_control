#include "RgbLedControl.hpp"

//SoftwareSerial bluetoothCommunicator(2, 3);
  
void RgbLedControl::setup()
{
  uint8_t index;
  
  // read current default play of light
  readEeprom();

  // read led properties for default play of light
  readEeprom(playOfLight);

  oldMillis = millis();

  randomSeed(analogRead(0));

//  bluetoothCommunicator.begin(9600);

  #ifdef PCA9685
  pwm.begin();
  #endif

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Setup completed");
  Serial.println();
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i ++)
    {
      Serial.println();
      #ifndef PCA9685
      led[i].setPin(pins[i]);
      Serial.print("Pin ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(led[i].getPin());
      #endif
      Serial.println();
    }
}

void RgbLedControl::loop()
{
  if (uint8_t durationOfPressing = button.getDurationOfPressing())
    {
      if (durationOfPressing > 4)
        {
          writeEeprom();
        }
      else
        {
          playOfLight ++;
          if (playOfLight >= numberOfPlays)
            {
              playOfLight = 0;
            }
          for (int i = 0; i < NUMBER_OF_LEDS; i ++)
            {
              led[i].setPointerIsChangeable(true);
              led[i].setFactor(0xff);
              led[i].setPointer(0xff);
            }
          readEeprom(playOfLight);
        }
    }
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i ++)
  {
    led[i].pointer2int();
    #ifdef PCA9685
    pwm.setPWM(led[i].getNumber(), 0, led[i].getIntensity());
    #else
    led[i].int2output();
    #endif
    if (led[i].letSpeedControlCount())
    {
      led[i].changePointer();
      led[i].pointer2int();
      if (led[i].getDarkerHasChanged())
      {
        led[i].setSpeedControlDuration(random(DURATION_MAX) + 1);
        if (led[i].getPointerIsAtMax())
          {
            if (led[i].getNewFactor())
              {
                led[i].setFactor((uint8_t)random(0xFF));
              }
            if (led[i].getNewMinPointerAtMax())
              {
                uint8_t number = (uint8_t)random(0xFF);
                if (number < led[i].getPointer())
                  {
                    led[i].setMinPointer(number);
                  }
              }
            if (led[i].getWaitAtMax())
              {
                if (random(2))
                  {
                    for (uint8_t j = 0; j < NUMBER_OF_LEDS; j ++)
                      {
                        if (j == i)
                          {
                            led[j].setPointerIsChangeable(false);
                          }
                        else
                          {
                            if (!(led[j].getPointerIsAtMin()))
                              {
                                led[j].setPointerIsChangeable(true);
                              }
                          }
                      }
                  }
              }
          }
        else if (led[i].getPointerIsAtMin())
          {
            if (led[i].getNewFactor())
              {
                led[i].setFactor((uint8_t)random(0xFF));
              }
            if (led[i].getNewMaxPointerAtMin())
              {
                uint8_t number = (uint8_t)random(0xFF);
                if (number > led[i].getPointer())
                  {
                    led[i].setMaxPointer(number);
                  }
              }
            if (led[i].getWaitAtMin())
              {
                if (random(2))
                  {
                    for (uint8_t j = 0; j < NUMBER_OF_LEDS; j ++)
                      {
                        if (j == i)
                          {
                            led[j].setPointerIsChangeable(false);
                          }
                        else
                          {
                            if (!(led[j].getPointerIsAtMax()))
                              {
                                led[j].setPointerIsChangeable(true);
                              }
                          }
                      }
                  }
              }
          }
      }
    }
  }
  loopDuration = millis() - oldMillis;
  while (millis() < (oldMillis + cycleTime));
  oldMillis = millis();
  if (Serial.available())
    {
      incomingByte = Serial.read();
      Serial.write(incomingByte);
      Serial.println();
      switch (incomingByte)
        {
          case 'a':
          case 'A':
            setPlayOfLight(false);
            info();
            break;
          case 'b':
          case 'B':
            break;
          case 'c':
          case 'C':
            setColorFactors();
            info();
            break;
          case 'd':
          case 'D':
            setDimmable();
            info();
            break;
          case 'e':
          case 'E':
            setNewMinPointerAtMax();
            info();
            break;
          case 'f':
          case 'F':
            setNewMaxPointerAtMin();
            info();
            break;
          case 'g':
          case 'G':
            info();
            break;
          case 'h':
          case 'H':
            help();
            break;
          case 'i':
          case 'I':
            info();
            break;
          case 'j':
          case 'J':
            setWaitAtMin();
            info();
            break;
          case 'k':
          case 'K':
            setWaitAtMax();
            info();
            break;
          case 'l':
          case 'L':
            changeLoopDuration(false);
            break;
          case 'm':
          case 'M':
            break;
          case 'n':
          case 'N':
            setNewFactor();
            info();
            break;
          case 'o':
          case 'O':
            setOffset(false);
            info();
            break;
          case 'p':
          case 'P':
            setIndex(false);
            info();
            break;
          case 'q':
          case 'Q':
            setNewFactor();
            info();
            break;
          case 'r':
          case 'R':
      	    setMinPointer();
            break;
          case 's':
          case 'S':
            setMaxPointer();
            break;
          case 't':
          case 'T':
            testAllLeds();
            break;
          case 'u':
          case 'U':
            break;
          case 'v':
          case 'V':
            writeEeprom();
            info();
            break;
          case 'w':
          case 'W':
            writeEeprom(playOfLight);
            info();
            break;
          case 'y':
          case 'Y':
            setNumberOfPlays();
            break;
          case 'z':
          case 'Z':
            break;
          default:
            break;
        }
    }
//    if (bluetoothCommunicator.available())
//    {
//      incomingByte = bluetoothCommunicator.read();
//      switch (incomingByte)
//        {
//        case 'd':
//        case 'D':
//          changeLoopDuration(true);
//          break;
//        case 'e':
//        case 'E':
//          setGlobalFactor(true);
//          break;
//        case 'a':
//        case 'A':
//          setPlayOfLight(true);
//          break;
//        default:
//          break;
//        }
//    }
}

unsigned char RgbLedControl::getPlayOfLight(void)
  {
    return playOfLight;
  }

void RgbLedControl::setPlayOfLight(bool bt)
  {
    byte incomingByte = 0;
    if (!bt)
      {
        incomingByte = (byte)getNumber();
        if (incomingByte < numberOfPlays)
          {
            playOfLight = incomingByte;
          }
      }

    for (int i = 0; i < NUMBER_OF_LEDS; i ++)
      {
        led[i].setPointerIsChangeable(true);
        led[i].setFactor(0xff);
        led[i].setPointer(0xff);
      }
    readEeprom(playOfLight);
  }

unsigned char RgbLedControl::getDefaultPlayOfLight(void)
  {
    return defaultPlayOfLight;
  }

void RgbLedControl::setDefaultPlayOfLight(bool bt)
  {
    byte incomingByte = 0;
    if (!bt)
      {
        incomingByte = (byte)getNumber();
        if (incomingByte < numberOfPlays)
          {
            defaultPlayOfLight = incomingByte;
          }
      }

    for (int i = 0; i < NUMBER_OF_LEDS; i ++)
      {
        led[i].setPointerIsChangeable(true);
        led[i].setFactor(0xff);
        led[i].setPointer(0xff);
      }
    readEeprom(playOfLight);
  }
  
unsigned char RgbLedControl::getNumberOfPlays(void)
  {
    return numberOfPlays;
  }

void RgbLedControl::setNumberOfPlays(void)
  {
    byte incomingByte = 0;
    incomingByte = (byte)getNumber();
    if (incomingByte < NUMBER_OF_PLAYS)
      {
        numberOfPlays = incomingByte;
      }
    EEPROM.write(ADDRESS_NUMBER_OF_PLAYS, numberOfPlays);
  }


void RgbLedControl::help()
{
  Serial.println();
  Serial.println("a: Play of light");
//  Serial.println("b: blue LED");
  Serial.println("c: color factor");
  Serial.println("d: dimmable");
  Serial.println("e: new min pointer at max");
  Serial.println("f: new max pointer at min");
//  Serial.println("g: green LED");
  Serial.println("h: Help");
  Serial.println("i: Info");
  Serial.println("j: wait at min");
  Serial.println("k: wait at max");
  Serial.println("l: loop duration");
//  Serial.println("m: wait at max 2");
  Serial.println("n: new factor");
  Serial.println("o: offset");
  Serial.println("p: progmem index");
  Serial.println("q: default play of ligth");
  Serial.println("r: min pointer");
  Serial.println("s: max pointer");
  Serial.println("t: Test all LEDs");
  Serial.println("u: global factor");
  Serial.println("v: Start with current play of light");
  Serial.println("w: Save current properties");
//  Serial.println("x: ");
  Serial.println("y: Set number of plays");
//  Serial.println("z: ");
  Serial.println();
}

void RgbLedControl::info()
{  
  Serial.println();
  Serial.println(F("intensity\tpointer\tpointer_min\tpointer_max\
\tdarker\tdimmable"));

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getIntensity());
      Serial.print("\t\t");
      Serial.print(led[i].getPointer());
      Serial.print("\t");
      Serial.print(led[i].getMinPointer());
      Serial.print("\t\t");
      Serial.print(led[i].getMaxPointer());
      Serial.print("\t\t");
      Serial.print(led[i].getDarker());
      Serial.print("\t");
      Serial.println(led[i].getDimmable());
    }

  Serial.println();
  Serial.println(F("PROGMEM_index\tdimFactor\tcolorFactor\
\toffset\tduration\tcounter"));

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getProgmemIndex());
      Serial.print("\t\t");
      Serial.print(led[i].getFactor());
      Serial.print("\t\t\t");
      Serial.print(led[i].getColorFactor());
      Serial.print("\t");
      Serial.print(led[i].getOffset());
      Serial.print("\t");
      Serial.print(led[i].getSpeedControlDuration());
      Serial.print("\t\t");
      Serial.println(led[i].getSpeedControlCounter());
    }

  Serial.println();
  Serial.println(F("newFactor\tnewMaxPointerAtMin\tnewMinPointerAtMax"));

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getNewFactor());
      Serial.print("\t\t");
      Serial.print(led[i].getNewMaxPointerAtMin());
      Serial.print("\t\t\t");
      Serial.println(led[i].getNewMinPointerAtMax());
    }

  Serial.println();
  Serial.println(F("waitAtMin\twaitAtMax\tpointerIsChangeable"));

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getWaitAtMin());
      Serial.print("\t\t");
      Serial.print(led[i].getWaitAtMax());
      Serial.print("\t\t");
      Serial.println(led[i].getPointerIsChangeable());
   }

  Serial.println();
  Serial.print(F("loop duration / ms:\t"));
  Serial.println(loopDuration);
  Serial.print(F("cycle time / ms:\t"));
  Serial.println(cycleTime);
  Serial.print(F("number of play of lights:\t"));
  Serial.println(numberOfPlays);
  Serial.print(F("Play of light:\t"));
  Serial.println(playOfLight);
  Serial.println();
}

int8_t RgbLedControl::getNumber()
{
  byte incomingNumber = 'g';
  int8_t number = -3;

  Serial.println();
  Serial.println(F("Enter a number"));

  do
    {
      while (Serial.available() <= 0);
      incomingNumber = Serial.read();

      switch (incomingNumber)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5': 
        case '6': 
        case '7':
        case '8':
        case '9':
          number = incomingNumber - ASCII_OFFSET;
          break;
        case 'a':
        case 'A':
          number = 10;
          break;
        case 'b':
        case 'B':
          number = 11;
          break;
        case 'c':
        case 'C':
          number = 12;
          break;
        case 'd': 
        case 'D':
          number = 13;
          break;
        case 'e':
        case 'E':
          number = 14;
          break;
        case 'f':
        case 'F':
          number = 15;
          break;
        case 'x':
        case 'X':
          number = -1;
          break;
        case 'q':
        case 'Q':
          number = -2;
          break;
        default:
          number = -3;
          break;
        }
    }while (number <= -3);
    
  Serial.println(number);
  return number;
}

int8_t RgbLedControl::getBoolean(void)
{
  byte incomingDigit = 'g';
  int8_t newBoolean = -3;

  Serial.println();
  Serial.println("Enter yes or no");

  do
    {

      while (Serial.available() <= 0);
      incomingDigit = Serial.read();

      switch (incomingDigit)
        {
        case '0':
        case 'n':
        case 'N':
          newBoolean = 0;
          break;
        case '1':
        case 'y':
        case 'Y':
        case 'j':
        case 'J':
          newBoolean = 1;
          break;
        case 'x':
        case 'X':
          newBoolean = -1;
          break;
        case 'q':
        case 'Q':
          newBoolean = -2;
          break;
        default:
          newBoolean = -3;
          break;
        }
    }while (newBoolean <= -3);

  Serial.println(newBoolean);
  return newBoolean;
}

void RgbLedControl::changeLoopDuration(bool bt)
{
  uint8_t newCycleTime = 0;
  byte incomingByte = ' ';
  
  if (bt)
  {
 //   if (bluetoothCommunicator.available())
 //   {
 //     incomingByte = bluetoothCommunicator.read() - ASCII_OFFSET;
 //   }
  }
  else
  {
    incomingByte = getNumber();
  }
  
  if ((incomingByte > 0) && (incomingByte < 16))
    {
      newCycleTime = incomingByte * 5;
      cycleTime = (unsigned long)newCycleTime;
    }
}


void RgbLedControl::setColorFactors()
  {
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
      {
        Serial.print("Factor ");
        Serial.println(": ");
        led[i].setColorFactor(17 * getNumber());
      }
  }

void RgbLedControl::setMinPointer()
  {
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
      {
        Serial.print("Factor ");
        Serial.println(": ");
        led[i].setMinPointer(17 * getNumber());
      }
  }

void RgbLedControl::setMaxPointer()
  {
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
      {
        Serial.print("Factor ");
        Serial.println(": ");
        led[i].setMaxPointer(17 * getNumber());
      }
  }

void RgbLedControl::setIndex(bool bt)
{
  int8_t newIndex = -1;

  if (bt)
  {
//    if (bluetoothCommunicator.available())
//    {
//      newIndex = bluetoothCommunicator.read() - ASCII_OFFSET;
//    }
  }
  else
  {
    newIndex = getNumber();
  }
  
  if ((newIndex >= 0) && (newIndex < NUMBER_OF_PROGMEMS))
    {
      for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
        {
            led[i].setProgmemIndex(newIndex);
        }
    }
}

void RgbLedControl::setOffset(bool bt)
  {
    uint8_t newOffset= 0;
    byte incomingByte = ' ';

    if (bt)
    {
//      if (bluetoothCommunicator.available())
//      {
//        incomingByte = bluetoothCommunicator.read() - ASCII_OFFSET;
//      }
    }
    else
    {
      incomingByte = getNumber();
    }

    if ((incomingByte >= 0) && (incomingByte < 16))
      {
        newOffset = incomingByte * 6;
        for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
          {
              led[i].setOffset(newOffset);
          }
      }
  }

void RgbLedControl::setDimmable(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setDimmable(getBoolean());
    }
}
void RgbLedControl::setWaitAtMin(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setWaitAtMin(getBoolean());
    }
}

void RgbLedControl::setWaitAtMax(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setWaitAtMax(getBoolean());
    }
}

void RgbLedControl::setNewFactor(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setNewFactor(getBoolean());
    }
}

void RgbLedControl::setNewMinPointerAtMax(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setNewMinPointerAtMax(getBoolean());
    }
}

void RgbLedControl::setNewMaxPointerAtMin(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setNewMaxPointerAtMin(getBoolean());
    }
}


void RgbLedControl::propertiesOfLed(uint8_t i)
  {
    byte incomingByte = ' ';
    bool isDimmable = false;

    Serial.println("Dimmable (y/n): ");
    isDimmable = getBoolean();
    if (isDimmable)
      {
        led[i].setDimmable(true);

        Serial.println("Color Factor (0..9, a..f): ");
        incomingByte = getNumber();
        if ((incomingByte >= 0) && (incomingByte < 16))
          {
              led[i].setColorFactor(incomingByte * 17);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("Offset (0..9, a..f): ");
        incomingByte = getNumber();
        if ((incomingByte >= 0) && (incomingByte < 16))
          {
            led[i].setOffset(incomingByte * 5);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("Progmem Array (0..5): ");
        incomingByte = getNumber();
        if ((incomingByte >= 0) && (incomingByte < NUMBER_OF_PROGMEMS))
          {
               led[i].setProgmemIndex(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("New factor (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setNewFactor(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("New min pointer at Max (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setNewMinPointerAtMax(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("New max pointer at Min (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setNewMaxPointerAtMin(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("Wait At Min (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setWaitAtMin(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("Wait At Max (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setWaitAtMax(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

      }
    else
      {
        led[i].setDimmable(false);
        led[i].setColorFactor(0xFF);
        led[i].setFactor(0xFF);
        led[i].setProgmemIndex(LINEAR);
        led[i].setPointer(getNumber() * 17);
        led[i].pointer2int();
        led[i].int2output();
      }
  END_PROPERTY:
    info();
  }

void RgbLedControl::readEeprom(void)
  {
    RgbDefaultProperties content;
    EEPROM.get (0, content);

    content.number_of_plays;
    content.play_at_por;
    content.number_of_leds;
    content.format_of_numbers;
    content.language;
  }

void RgbLedControl::readEeprom(uint8_t play)
{
  uint8_t addressStart = sizeof(RgbDefaultProperties) + play * (1 + sizeof(LedDefaultProperties));
  uint8_t address = addressStart;

  Serial.print(F("Current play of light: "));
  Serial.println(playOfLight);
  Serial.println();

  Serial.println(ADDRESS_NUMBER_OF_DEFAULT_PLAY);
  Serial.print(F("Number of default play: "));
  Serial.println(EEPROM.read(ADDRESS_NUMBER_OF_DEFAULT_PLAY));
  Serial.println();

  Serial.println(ADDRESS_NUMBER_OF_PLAYS);
  Serial.print(F("Number of plays: "));
  Serial.println(EEPROM.read(ADDRESS_NUMBER_OF_PLAYS));
  Serial.println();

  Serial.print(F("Start address: "));
  Serial.println(addressStart);

  Serial.println(F("Reading LED proberties from the EEPROM..."));
  Serial.println(address);
  cycleTime = (unsigned long)EEPROM.read(address + OFFSET_LOOP_DURATION);
  if ((cycleTime == 0xFF) || (cycleTime == 0))
    {
      cycleTime = (unsigned long)DELAY_TIME;
    }
  Serial.print(F("Time per cycle"));
  Serial.println(cycleTime);
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].loadFromEeprom(address + i * sizeof(LedDefaultProperties));
    }
}

void RgbLedControl::writeEeprom(void)
{
  EEPROM.write(ADDRESS_NUMBER_OF_DEFAULT_PLAY, playOfLight);
  EEPROM.write(ADDRESS_NUMBER_OF_PLAYS, numberOfPlays);
}

void RgbLedControl::writeEeprom(uint8_t play)
{
  uint8_t content;
  uint8_t addressStart = 2 + play * (LENGTH_OF_PLAY_PROPERTIES);
  uint8_t address = addressStart;

  Serial.print(F("Saving LED properties for Play of Light: "));
  Serial.println(play);
  Serial.println();

  Serial.println(address);
  EEPROM.write(address + OFFSET_LOOP_DURATION, (uint8_t)cycleTime);
  Serial.print(F("Time per cycle: "));
  Serial.println(cycleTime);
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.println(address);
      content = led[i].getOffset();
      EEPROM.write(address, content);
      Serial.print(F("Offset: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getProgmemIndex();
      EEPROM.write(address, content);
      Serial.print(F("Progmem Index"));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getDimmable();
      EEPROM.write(address, content);
      Serial.print("Dimmable: ");
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = led[i].getMinPointer();
      EEPROM.write(address, content);
      Serial.print(F("Min. Pointer: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = led[i].getMaxPointer();
      EEPROM.write(address, content);
      Serial.print(F("Max. Pointer: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getNewFactor();
      EEPROM.write(address, content);
      Serial.print(F("New Factor: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getNewMinPointerAtMax();
      EEPROM.write(address, content);
      Serial.print(F("New min pointer at max: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getNewMaxPointerAtMin();
      EEPROM.write(address, content);
      Serial.print(F("New max pointer at min: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getWaitAtMin();
      EEPROM.write(address, content);
      Serial.print(F("Wait at min: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getWaitAtMax();
      EEPROM.write(address, content);
      Serial.print(F("Wait at max: "));
      Serial.println(content);
      address ++;

      Serial.println();
    }
}

void RgbLedControl::testAllLeds(void)
{
  Serial.println("Test of all LEDs:");
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print("LED ");
      Serial.println(i);
      for (uint8_t j = 0; j < NUMBER_OF_LEDS; j++)
        {
          led[j].setIntensity(0);
          switch(j)
          {
          case 0:
            analogWrite(3, led[0].getIntensity());
            break;
          case 1:
            analogWrite(5, led[1].getIntensity());
            break;
          case 2:
            analogWrite(6, led[2].getIntensity());
            break;
          }
        }
      delay(1000);
      for (uint8_t k = 1; k < 21; k++)
        {
          led[i].setIntensity(k);
          Serial.println(k);
          switch(i)
          {
          case 0:
            analogWrite(3, led[0].getIntensity());
            break;
          case 1:
            analogWrite(5, led[1].getIntensity());
            break;
          case 2:
            analogWrite(6, led[2].getIntensity());
            break;
          }
          delay(1000);
        }
      Serial.println();
    }
}
