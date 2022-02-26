#include "RgbLedControl.hpp"

SoftwareSerial bluetoothCommunicator(2, 3);
  
void RgbLedControl::setup()
{
  uint8_t index;

  led[0].setColor('r');
  led[1].setColor('g');
  led[2].setColor('b');

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i ++)
  {
    led[i].setNumber(i);
    led[i].setFactor(0xFF);
    led[i].setGlobalFactor(0xFF);
    led[i].setNewFactorAtMin(false);
    led[i].setNewFactorAtMax(true);
    led[i].setNewMaxPointerAtMin(false);
    led[i].setNewMinPointerAtMax(false);
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
  
  bluetoothCommunicator.begin(9600);

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
            writeEeprom();
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
            break;
          case 'w':
          case 'W':
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
    if (bluetoothCommunicator.available())
    {
      incomingByte = bluetoothCommunicator.read();
      switch (incomingByte)
        {
        case 'd':
        case 'D':
          changeLoopDuration(true);
          break;
        case 'e':
        case 'E':
          setGlobalFactor(true);
          break;
        case 'a':
        case 'A':
          setPlayOfLight(true);
          break;
        default:
          break;
        }
    }
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
        if (incomingByte < NUMBER_OF_PLAYS)
          {
            playOfLight = incomingByte;
          }
      }
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
  Serial.println("s: Save values to EEPROM");
  Serial.println("t: Test all LEDs");
  Serial.println("u: global factor");
//  Serial.println("v: ");
//  Serial.println("w: ");
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
  Serial.println("newFactorAtMin\tnewFactorAtMax\tnewMaxPointerAtMin\
\tnewMinPointerAtMax");

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
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
    if (bluetoothCommunicator.available())
    {
      incomingByte = bluetoothCommunicator.read() - ASCII_OFFSET;
    }
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
    if (bluetoothCommunicator.available())
    {
      incomingByte = bluetoothCommunicator.read() - ASCII_OFFSET; 
    }
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
    if (bluetoothCommunicator.available())
    {
      newIndex = bluetoothCommunicator.read() - ASCII_OFFSET;
    }
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
      if (bluetoothCommunicator.available())
      {
        incomingByte = bluetoothCommunicator.read() - ASCII_OFFSET;
      }
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
            led[i].setNewFactorAtMax(getBoolean());
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
        led[i].setNewFactorAtMin(getBoolean());
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
  uint8_t index;
  uint8_t global_factor;
  
  cycleTime = EEPROM.read(ADDRESS_LOOP_TIME);
  if (cycleTime == 0xFF)
    {
      cycleTime = DELAY_TIME;
    }
  
  global_factor = EEPROM.read(ADDRESS_GLOBAL_FACTOR);
  if ((global_factor == 0xFF)||(global_factor == 0x00))
    {
      global_factor = DEFAULT_GLOBAL_FACTOR;
    }
  
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setGlobalFactor(global_factor);
    }
  
  index = EEPROM.read(ADDRESS_PROGMEM_INDEX);
  if (index == 0xFF)
    {
      index = DEFAULT_PROGMEM_NUMBER;
    }
  
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setProgmemIndex(index);
    }
}

void RgbLedControl::writeEeprom()
{
  EEPROM.write(ADDRESS_LOOP_TIME, cycleTime);
  EEPROM.write(ADDRESS_GLOBAL_FACTOR, led[0].getGlobalFactor());
  EEPROM.write(ADDRESS_PROGMEM_INDEX, led[0].getProgmemIndex());
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
