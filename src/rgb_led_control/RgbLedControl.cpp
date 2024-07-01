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

  Serial.begin(9600);
  while (!Serial);
  Serial.print(F("Size of RGB Control struct: "));
  Serial.println(sizeof(RgbDefaultProperties));
  Serial.print(F("Siye of Led struct: "));
  Serial.println(sizeof(LedDefaultProperties));
  Serial.print(F("Size of play of light: "));
  Serial.println((1 + NUMBER_OF_LEDS * sizeof(LedDefaultProperties)));
  Serial.println(F("Setup completed"));
  Serial.println();

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
      {
        led[i].setPin(i);
        #ifndef PCA
          led[i].setPin2default();
        #endif
      }


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
          for (int i = 0; i < NUMBER_OF_LEDS; i ++)
            {
              if (led[i].getLedIsOn()) {
                led[i].setIntensity(0xFF);
              }
              else {
                led[i].setIntensity(0x00);
              }
              led[i].int2output();
            }
        }
    }
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i ++)
  {
    if (led[i].getDimmable())
      {
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
    #ifdef PCA9685
    pwm.setPWM(led[i].getNumber(), 0, led[i].getIntensity());
    #else
    led[i].int2output();
    #endif
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
            setLedIsOn();
            info();
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
        if (incomingByte <= numberOfPlays)
          {
            playOfLight = incomingByte - 1;
          }
      }

    for (int i = 0; i < NUMBER_OF_LEDS; i ++)
      {
        led[i].setPointerIsChangeable(true);
        led[i].setFactor(0xff);
        led[i].setPointer(0xff);
      }
    readEeprom(playOfLight);
    for (int i = 0; i < NUMBER_OF_LEDS; i ++)
      {
        if (led[i].getLedIsOn()) {
          led[i].setIntensity(0xFF);
        }
        else {
          led[i].setIntensity(0x00);
        }
        led[i].int2output();
      }
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
            writeEeprom();
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
    writeEeprom();
  }


void RgbLedControl::help()
{
  Serial.println();
  Serial.println(F("a: Play of light"));
//  Serial.println("b: blue LED");
  Serial.println(F("c: color factor"));
  Serial.println(F("d: dimmable"));
  Serial.println(F("e: new min pointer at max"));
  Serial.println(F("f: new max pointer at min"));
//  Serial.print(Fln("g: green LED");
  Serial.println(F("h: Help"));
  Serial.println(F("i: Info"));
  Serial.println(F("j: wait at min"));
  Serial.println(F("k: wait at max"));
  Serial.println(F("l: loop duration"));
//  Serial.print(Fln("m: wait at max 2");
  Serial.println(F("n: new factor"));
  Serial.println(F("o: offset"));
  Serial.println(F("p: progmem index"));
  Serial.println(F("q: default play of ligth"));
  Serial.println(F("r: min pointer"));
  Serial.println(F("s: max pointer"));
  Serial.println(F("v: Start with current play of light"));
  Serial.println(F("w: Save current properties"));
//  Serial.println("x: ");
  Serial.println(F("y: Set number of plays"));
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
  Serial.println(F("newFactor\tnewMaxPointerAtMin\tnewMinPointerAtMax\tOn"));

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getNewFactor());
      Serial.print("\t\t");
      Serial.print(led[i].getNewMaxPointerAtMin());
      Serial.print("\t\t\t");
      Serial.print(led[i].getNewMinPointerAtMax());
      Serial.print("\t\t");
      Serial.println(led[i].getLedIsOn());
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
  Serial.println(playOfLight + 1);
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

//  if (bt)
//  {
////    if (bluetoothCommunicator.available())
////    {
////      newIndex = bluetoothCommunicator.read() - ASCII_OFFSET;
////    }
//  }
//  else
//  {
//    newIndex = getNumber();
//  }
  
//  if ((newIndex >= 0) && (newIndex < NUMBER_OF_PROGMEMS))
    {
      for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
        {
            led[i].setProgmemIndex(17 * getNumber());
        }
    }
}

void RgbLedControl::setOffset(bool bt)
  {
//    uint8_t newOffset= 0;
//    byte incomingByte = ' ';
//
//    if (bt)
//    {
////      if (bluetoothCommunicator.available())
////      {
////        incomingByte = bluetoothCommunicator.read() - ASCII_OFFSET;
////      }
//    }
//    else
//    {
//      incomingByte = getNumber();
//    }

//    if ((incomingByte >= 0) && (incomingByte < 16))
      {
        for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
          {
              led[i].setOffset(getNumber());
          }
      }
  }

void RgbLedControl::setDimmable(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if (getBoolean())
        {
          led[i].setDimmable(true);
        }
      else
        {
          led[i].setDimmable(false);
          led[i].setIntensity(17 * getNumber());
          led[i].int2output();
        }
    }
}

void RgbLedControl::setLedIsOn(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if (getBoolean())
        {
          led[i].setLedIsOn(true);
        }
      else
        {
          led[i].setLedIsOn(false);
          led[i].setDimmable(false);
          led[i].setIntensity(0);
          led[i].int2output();
        }
    }
}

void RgbLedControl::setWaitAtMin(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if (getBoolean())
        {
          led[i].setWaitAtMin(true);
        }
      else
        {
          led[i].setWaitAtMin(false);
          led[i].setPointerIsChangeable(true);
        }
    }
}

void RgbLedControl::setWaitAtMax(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if (getBoolean())
        {
          led[i].setWaitAtMax(true);
        }
      else
        {
          led[i].setWaitAtMax(false);
          led[i].setPointerIsChangeable(true);
        }
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

void RgbLedControl::readEeprom(void)
  {
    RgbDefaultProperties content;
    EEPROM.get (0, content);

    numberOfPlays = content.number_of_plays;
    defaultPlayOfLight = content.play_at_por;
//    number_of_leds_ = content.number_of_leds;
//    format_of_numbers_ = content.format_of_numbers;
//    language_ = content.language;
  }

void RgbLedControl::readEeprom(uint8_t play)
{
  uint8_t addressStart = sizeof(RgbDefaultProperties) + play * (1 + sizeof(LedDefaultProperties) * NUMBER_OF_LEDS);
  uint8_t address = addressStart;
/*
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
  Serial.println(address);*/
  cycleTime = (unsigned long)EEPROM.read(address + OFFSET_LOOP_DURATION);
  if ((cycleTime == 0xFF) || (cycleTime == 0))
    {
      cycleTime = (unsigned long)DELAY_TIME;
    }
//  Serial.print(F("Time per cycle"));
//  Serial.println(cycleTime);
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].loadFromEeprom(address + i * sizeof(LedDefaultProperties));
    }
}

void RgbLedControl::writeEeprom(void)
{
    RgbDefaultProperties content;

    content.number_of_plays = numberOfPlays;
    content.play_at_por = defaultPlayOfLight;
    content.number_of_leds;
    content.format_of_numbers;
    content.language;
    content.button_pressed;

    EEPROM.put (0, content);
}

void RgbLedControl::writeEeprom(uint8_t play)
{
  uint8_t content;
  uint8_t addressStart = sizeof(RgbDefaultProperties) + play * (1 + sizeof(LedDefaultProperties) * NUMBER_OF_LEDS);
  uint8_t address = addressStart;
/*
  Serial.print(F("Saving LED properties for Play of Light: "));
  Serial.println(play);
  Serial.println();

  Serial.println(address);*/
  EEPROM.write(address + OFFSET_LOOP_DURATION, (uint8_t)cycleTime);
/*  Serial.print(F("Time per cycle: "));
  Serial.println(cycleTime);*/
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].saveToEeprom(address + i * sizeof(LedDefaultProperties));
    }
}
