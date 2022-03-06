#include "RgbLedControl.hpp"

//SoftwareSerial bluetoothCommunicator(2, 3);
  
void RgbLedControl::setup()
{
  uint8_t index;

  led[0].setColor('r');
  led[1].setColor('g');
  led[2].setColor('b');

  readEeprom();
  readEeprom(playOfLight);

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i ++)
  {
    led[i].setNumber(i);
    led[i].setFactor(0xFF);
    led[i].setGlobalFactor(0xFF);
    led[i].setControlViaPointer(true);
    led[i].setNewFactorAtMin(false);
    led[i].setNewFactorAtMax(false);
    led[i].setNewMaxPointerAtMin(true);
    led[i].setNewMinPointerAtMax(true);
    led[i].setWaitAtMax1(false);
    led[i].setWaitAtMin1(false);
    led[i].setWaitAtMax2(true);
    led[i].setWaitAtMin2(false);
    led[i].setProgmemIndex(0);
    #ifndef PCA9685
    led[i].setPin2default();
    #endif
  }



  oldMillis = millis();

  randomSeed(analogRead(0));

  readEeprom();
  
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
      Serial.print("Number: ");
      Serial.println(led[i].getNumber());
      Serial.print("Color: ");
      Serial.write(led[i].getColor());
      Serial.println();
      #ifndef PCA9685
      Serial.print("Pin: ");
      Serial.println(led[i].getPin());
      #endif
      Serial.println();
    }
}

void RgbLedControl::loop()
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i ++)
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
            if (led[i].getNewFactorAtMax())
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
            if (led[i].getWaitAtMax1())
              {
                if (countLedsGettingDarker() == NUMBER_OF_LEDS)
                  {
                    led[i].setDimmable(false);
                  }
                else
                  {
                    led[i].setDimmable(true);
                  }
              }
            if (led[i].getWaitAtMax2())
              {
                if (random(2))
                  {
                    for (uint8_t j = 0; j < NUMBER_OF_LEDS; j ++)
                      {
                        if (j == i)
                          {
                            led[j].setDimmable(false);
                          }
                        else
                          {
                            if (!(led[j].getPointerIsAtMin()))
                              {
                                led[j].setDimmable(true);
                              }
                          }
                      }
                  }
              }
          }
        else if (led[i].getPointerIsAtMin())
          {
            if (led[i].getNewFactorAtMin())
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
            if (led[i].getWaitAtMin1())
              {
                if (countLedsGettingDarker() == 0)
                  {
                    led[i].setDimmable(false);
                  }
                else
                  {
                    led[i].setDimmable(true);
                  }
              }
            if (led[i].getWaitAtMin2())
              {
                if (random(2))
                  {
                    for (uint8_t j = 0; j < NUMBER_OF_LEDS; j ++)
                      {
                        if (j == i)
                          {
                            led[j].setDimmable(false);
                          }
                        else
                          {
                            if (!(led[j].getPointerIsAtMax()))
                              {
                                led[j].setDimmable(true);
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
            blueLED();
            break;
          case 'c':
          case 'C':
            setColorFactors();
            info();
            break;
          case 'd':
          case 'D':
            changeLoopDuration(false);
            info();
            break;
          case 'e':
          case 'E':
            toggleNewMinPointerAtMax();
            info();
            break;
          case 'f':
          case 'F':
            toggleNewMaxPointerAtMin();
            info();
            break;
          case 'g':
          case 'G':
            greenLED();
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
            toggleWaitAtMin1();
            info();
            break;
          case 'k':
          case 'K':
            toggleWaitAtMax1();
            info();
            break;
          case 'l':
          case 'L':
            toggleWaitAtMin2();
            info();
            break;
          case 'm':
          case 'M':
            toggleWaitAtMax2();
            info();
            break;
          case 'n':
          case 'N':
            toggleNewFactorAtMax();
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
            toggleNewFactorAtMin();
            info();
            break;
          case 'r':
          case 'R':
            redLED();
            info();
            break;
          case 's':
          case 'S':
            toggleControlViaPointers();
            break;
          case 't':
          case 'T':
            testAllLeds();
            break;
          case 'u':
          case 'U':
            setGlobalFactor(false);
            info();
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
          case 'x':
          case 'X':
            break;
          case 'y':
          case 'Y':
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
//    if (!bt)
//      {
        incomingByte = (byte)getNumber();
        if (incomingByte < NUMBER_OF_PLAYS)
          {
            playOfLight = incomingByte;
          }
//      }
    readEeprom(playOfLight);
  }

void RgbLedControl::help()
{
  Serial.println();
  Serial.println("a: Play of light");
  Serial.println("b: blue LED");
  Serial.println("c: color factor");
  Serial.println("d: loop duration");
  Serial.println("e: new min pointer at max");
  Serial.println("f: new max pointer at min");
  Serial.println("g: green LED");
  Serial.println("h: Help");
  Serial.println("i: Info");
  Serial.println("j: wait at min 1");
  Serial.println("k: wait at max 1");
  Serial.println("l: wait at min 2");
  Serial.println("m: wait at max 2");
  Serial.println("n: new factor at max");
  Serial.println("o: offset");
  Serial.println("p: progmem index");
  Serial.println("q: new factor at min");
  Serial.println("r: red LED");
  Serial.println("s: control via pointers");
  Serial.println("t: Test all LEDs");
  Serial.println("u: global factor");
  Serial.println("v: Start with current play of light");
  Serial.println("w: Save current properties");
//  Serial.println("x: ");
//  Serial.println("y: ");
//  Serial.println("z: ");
  Serial.println();
}

void RgbLedControl::info()
{  
  Serial.println();
  Serial.println("number\tcolor\tintensity\tpointer\tpointer_min\tpointer_max\
\tdarker\tdimmable");

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getNumber());
      Serial.print("\t");
      Serial.write(led[i].getColor());
      Serial.print("\t");
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
  Serial.println("PROGMEM_index\tfactor\tglobalFactor\tcolorFactor\
\toffset\tduration\tcounter");

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getProgmemIndex());
      Serial.print("\t\t");
      Serial.print(led[i].getFactor());
      Serial.print("\t\t");
      Serial.print(led[i].getGlobalFactor());
      Serial.print("\t\t");
      Serial.print(led[i].getColorFactor());
      Serial.print("\t");
      Serial.print(led[i].getOffset());
      Serial.print("\t");
      Serial.print(led[i].getSpeedControlDuration());
      Serial.print("\t\t");
      Serial.println(led[i].getSpeedControlCounter());
    }

  Serial.println();
  Serial.println("controlViaPointer\tnewFactorAtMin\tnewFactorAtMax\
\tnewMaxPointerAtMin\tnewMinPointerAtMax");

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getControlViaPointer());
      Serial.print("\t\t\t");
      Serial.print(led[i].getNewFactorAtMin());
      Serial.print("\t\t");
      Serial.print(led[i].getNewFactorAtMax());
      Serial.print("\t\t");
      Serial.print(led[i].getNewMaxPointerAtMin());
      Serial.print("\t\t\t");
      Serial.println(led[i].getNewMinPointerAtMax());
    }

  Serial.println();
  Serial.println("waitAtMin1\twaitAtMax1\twaitAtMin2\twaitAtMax2");

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getWaitAtMin1());
      Serial.print("\t\t");
      Serial.print(led[i].getWaitAtMax1());
      Serial.print("\t\t");
      Serial.print(led[i].getWaitAtMin2());
      Serial.print("\t\t");
      Serial.println(led[i].getWaitAtMax2());
   }

  Serial.println();
  Serial.print("loop duration / ms:\t");
  Serial.println(loopDuration);
  Serial.print("cycle time / ms:\t");
  Serial.println(cycleTime);
  Serial.print("Play of light:\t");
  Serial.println(playOfLight);
  Serial.println();
}

int8_t RgbLedControl::getNumber()
{
  byte incomingNumber = 'g';
  int8_t number = -3;

  Serial.println();
  Serial.println("Enter a number");

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

void RgbLedControl::setGlobalFactor(bool bt)
{
  uint8_t newGlobalFactor = 0xFF;
  byte incomingByte = ' ';

  if (bt)
  {
//    if (bluetoothCommunicator.available())
//    {
//      incomingByte = bluetoothCommunicator.read() - ASCII_OFFSET;
//    }
  }
  else
  {
    incomingByte = getNumber();
  }

  if (incomingByte <= 4)
    {
        newGlobalFactor = 175 + incomingByte * 20;
  
        for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
        {
          led[i].setGlobalFactor(newGlobalFactor);
        }
    }
}

void RgbLedControl::setColorFactors()
  {
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
      {
        Serial.print("Factor ");
        Serial.write(led[i].getColor());
        Serial.println(": ");
        led[i].setColorFactor(17 * getNumber());
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

void RgbLedControl::toggleControlViaPointers(void)
  {
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
      {
        led[i].toggleControlViaPointer();
      }
  }

void RgbLedControl::toggleNewFactorAtMax(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleNewFactorAtMax();
    }
}

void RgbLedControl::toggleNewFactorAtMin(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleNewFactorAtMin();
    }
}

void RgbLedControl::toggleNewMinPointerAtMax(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleNewMinPointerAtMax();
    }
}

void RgbLedControl::toggleNewMaxPointerAtMin(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleNewMaxPointerAtMin();
    }
}

void RgbLedControl::toggleWaitAtMax1(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleWaitAtMax1();
    }
}

void RgbLedControl::toggleWaitAtMin1(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleWaitAtMin1();
    }
}

void RgbLedControl::toggleWaitAtMax2(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleWaitAtMax2();
    }
}

void RgbLedControl::toggleWaitAtMin2(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleWaitAtMin2();
    }
}

uint8_t RgbLedControl::countLedsGettingDarker(void)
  {
    uint8_t sum = 0;

    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
      {
        if (led[i].getDarker())
          {
            sum ++;
          }
      }

    return sum;
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

        Serial.println("New factor at Max (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setNewFactorAtMax(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("New factor at Min (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setNewFactorAtMin(incomingByte);
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

        Serial.println("Wait At Min 1 (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setWaitAtMin1(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("Wait At Max 1 (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setWaitAtMax1(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("Wait At Min 2 (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setWaitAtMin2(incomingByte);
          }
        else if (incomingByte == -2)
          {
            goto END_PROPERTY;
          }

        Serial.println("Wait At Max 2 (y/n): ");
        incomingByte = getBoolean();
        if ((incomingByte == 0) ||(incomingByte == 1))
          {
            led[i].setWaitAtMax2(incomingByte);
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
      }
  END_PROPERTY:
    info();
  }

void RgbLedControl::blueLED(void)
  {
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
      {
        if ((led[i].getColor() == 'b') || (led[i].getColor() == 'B'))
          {
            propertiesOfLed(i);
          }
      }
  }

void RgbLedControl::greenLED(void)
  {
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
      {
        if ((led[i].getColor() == 'g') || (led[i].getColor() == 'G'))
          {
            propertiesOfLed(i);
          }
      }
  }

void RgbLedControl::redLED(void)
  {
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
      {
        if ((led[i].getColor() == 'r') || (led[i].getColor() == 'R'))
          {
            propertiesOfLed(i);
          }
      }
  }

void RgbLedControl::readEeprom(void)
  {
    playOfLight = EEPROM.read(ADDRESS_NUMBER_OF_PLAY);
    if (playOfLight > MAX_NUMBER_OF_PLAYS)
      {
        playOfLight = DEFAULT_PLAY_OF_LIGHT;
      }
  }

void RgbLedControl::readEeprom(uint8_t play)
{
  uint8_t global_factor;
  uint8_t content;
  uint8_t address = 1 + play * LENGTH_OF_PLAY_PROPERTIES;

  Serial.println("Reading LED proberties from EEPROM...");
  loopDuration = (unsigned long)EEPROM.read(address + OFFSET_LOOP_DURATION);
  if ((loopDuration == 0xFF) || (loopDuration == 0))
    {
      loopDuration = (unsigned long)DELAY_TIME;
    }
  Serial.print("Loop Duration: ");
  Serial.println(loopDuration);
  address ++;

  global_factor = EEPROM.read(address + OFFSET_GLOBAL_FACTOR);
  if ((global_factor == 0xFF)||(global_factor == 0x00))
    {
      global_factor = DEFAULT_GLOBAL_FACTOR;
    }
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setGlobalFactor(global_factor);
    }
  Serial.print("Global Factor: ");
  Serial.println(global_factor);
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if ((led[i].getColor() == 'R') || (led[i].getColor() == 'r'))
        {
          address = OFFSET_RED;
          Serial.println("Red LED:");
        }
      if ((led[i].getColor() == 'G') || (led[i].getColor() == 'g'))
        {
          address = OFFSET_GREEN;
          Serial.println("Green LED:");
        }
      if ((led[i].getColor() == 'B') || (led[i].getColor() == 'b'))
        {
          address = OFFSET_BLUE;
          Serial.println("Blue LED:");
        }

      content = EEPROM.read(address);
      led[i].setColorFactor(content);
      Serial.print("Color Factor: ");
      Serial.println(content);
      address ++;

      content = EEPROM.read(address);
      led[i].setOffset(content);
      Serial.print("Offset: ");
      Serial.println(content);
      address ++;

      content = EEPROM.read(address);
      if (content == 0xFF)
        {
          led[i].setMinPointer(0);
        }
      else
        {
          led[i].setMinPointer(content);
        }
      Serial.print("Min. Pointer: ");
      Serial.println(content);
      address ++;

      content = EEPROM.read(address);
      led[i].setMaxPointer(content);
      Serial.print("Max. Pointer: ");
      Serial.println(content);
      address ++;

      content = EEPROM.read(address);
      if (content & NEW_FACTOR_AT_MIN)
        {
          led[i].setNewFactorAtMin(true);
        }
      else
        {
          led[i].setNewFactorAtMin(false);
        }
      if (content & NEW_FACTOR_AT_MAX)
        {
          led[i].setNewFactorAtMax(true);
        }
      else
        {
          led[i].setNewFactorAtMax(false);
        }
      if(content & NEW_MIN_POINTER_AT_MAX)
        {
          led[i].setNewMinPointerAtMax(true);
        }
      else
        {
          led[i].setNewMinPointerAtMax(false);
        }
      if (content & NEW_MAX_POINTER_AT_MIN)
        {
          led[i].setNewMaxPointerAtMin(true);
        }
      else
        {
          led[i].setNewMaxPointerAtMin(false);
        }
      if (content & WAIT_AT_MIN1)
        {
          led[i].setWaitAtMin1(true);
        }
      else
        {
          led[i].setWaitAtMin1(false);
        }
      if (content & WAIT_AT_MAX1)
        {
          led[i].setWaitAtMax1(true);
        }
      else
        {
          led[i].setWaitAtMax1(false);
        }
      if (content & WAIT_AT_MIN2)
        {
          led[i].setWaitAtMin2(true);
        }
      else
        {
          led[i].setWaitAtMin2(false);
        }
      if (content & WAIT_AT_MAX2)
        {
          led[i].setWaitAtMax2(true);
        }
      else
        {
          led[i].setWaitAtMax2(false);
        }
      address ++;
      Serial.print("New factor, pointers and waits: ");
      Serial.println(content);
      address ++;

      content = EEPROM.read(address);
      if (content & DIMMABLE)
        {
          led[i].setDimmable(true);
        }
      else
        {
          led[i].setDimmable(false);
        }
      led[i].setProgmemIndex(content & 0x0F);
      Serial.print("Dimmable and index: ");
      Serial.println(content);
      Serial.println();
    }
}

void RgbLedControl::writeEeprom(void)
{
  EEPROM.write(ADDRESS_NUMBER_OF_PLAY, playOfLight);
}

void RgbLedControl::writeEeprom(uint8_t play)
{
  uint8_t cycleTime;
  uint8_t global_factor;
  uint8_t content;
  uint8_t address = 1 + play * LENGTH_OF_PLAY_PROPERTIES;

  Serial.print("Saving LED properties for Play of Light: ");
  Serial.println(play);
  Serial.println();

  EEPROM.write(address + OFFSET_LOOP_DURATION, (uint8_t)loopDuration);
  Serial.print("Loop duration: ");
  Serial.println(loopDuration);
  address ++;

  content = led[0].getGlobalFactor();
  EEPROM.write(address, content);
  Serial.print("Global factor: ");
  Serial.println(content);
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if ((led[i].getColor() == 'R') || (led[i].getColor() == 'r'))
        {
          address = OFFSET_RED;
          Serial.println("Update red LED: ");
        }
      if ((led[i].getColor() == 'G') || (led[i].getColor() == 'g'))
        {
          address = OFFSET_GREEN;
          Serial.println("Update green LED: ");
        }
      if ((led[i].getColor() == 'B') || (led[i].getColor() == 'b'))
        {
          address = OFFSET_BLUE;
          Serial.println("Update blue LED: ");
        }

      content = led[i].getColorFactor();
      EEPROM.write(address, content);
      Serial.print("Color Factor: ");
      Serial.println(content);
      address ++;

      content = led[i].getOffset();
      EEPROM.write(address, content);
      Serial.print("Offset: ");
      Serial.println(content);
      address ++;

      content = led[i].getMinPointer();
      EEPROM.write(address, content);
      Serial.print("Min. Pointer: ");
      Serial.println(content);
      address ++;

      content = led[i].getMaxPointer();
      EEPROM.write(address, content);
      Serial.print("Max. Pointer: ");
      Serial.println(content);
      address ++;

      content = 0;
      if (led[i].getNewFactorAtMin())
        {
          content = content | NEW_FACTOR_AT_MIN;
        }
      if (led[i].getNewFactorAtMax())
        {
          content = content | NEW_FACTOR_AT_MAX;
        }
      if(led[i].getNewMinPointerAtMax())
        {
          content = content | NEW_MIN_POINTER_AT_MAX;
        }
      if (led[i].getNewMaxPointerAtMin())
        {
          content = content | NEW_MAX_POINTER_AT_MIN;
        }
      if (led[i].getWaitAtMin1())
        {
          content = content | WAIT_AT_MIN1;
        }
      if (led[i].getWaitAtMax1())
        {
          content = content | WAIT_AT_MAX1;
        }
      if (led[i].getWaitAtMin2())
        {
          content = content | WAIT_AT_MIN2;
        }
      if (led[i].getWaitAtMax2())
        {
          content = content | WAIT_AT_MAX2;
        }
      EEPROM.write(address, content);
      Serial.print("New factor, pointers and waits: ");
      Serial.println(content);
      address ++;

      content = led[i].getProgmemIndex();
      if (led[i].getDimmable())
        {
          content = content | DIMMABLE;
        }
      EEPROM.write(address, content);
      Serial.print("Dimmable and index: ");
      Serial.println(content);
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
