#include "RgbLedControl.hpp"

//SoftwareSerial bluetoothCommunicator(2, 3);
  
void RgbLedControl::setup()
{
  uint8_t index;

  led[0].setColor('r');
  led[1].setColor('g');
  led[2].setColor('b');

  // read current default play of light
  readEeprom();

  // read led properties for default play of light
  readEeprom(playOfLight);

for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
  {
    led[i].setNumber(i);
    #ifndef PCA
      led[i].setPin2default();
    #endif
  }



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
            }
          readEeprom(playOfLight);
        }
    }
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
                    led[i].setPointerIsChangeable(false);
                  }
                else
                  {
                    led[i].setPointerIsChangeable(true);
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
                    led[i].setPointerIsChangeable(false);
                  }
                else
                  {
                    led[i].setPointerIsChangeable(true);
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
            writeEeprom(playOfLight);
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
        if (incomingByte < NUMBER_OF_PLAYS)
          {
            playOfLight = incomingByte;
          }
      }

    for (int i = 0; i < NUMBER_OF_LEDS; i ++)
      {
        led[i].setPointerIsChangeable(true);
        led[i].setFactor(0xff);
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
    if (incomingByte < numberOfPlays)
      {
        numberOfPlays = incomingByte;
      }
    EEPROM.write(ADDRESS_NUMBER_OF_PLAYS, numberOfPlays);
  }


void RgbLedControl::help()
{
  Serial.println();
  Serial.println(F("a: Play of light"));
  Serial.println(F("b: blue LED"));
  Serial.println(F("c: color factor"));
  Serial.println(F("d: loop duration / cycle time"));
  Serial.println(F("e: new min pointer at max"));
  Serial.println(F("f: new max pointer at min"));
  Serial.println(F("g: green LED"));
  Serial.println(F("h: Help"));
  Serial.println(F("i: Info"));
  Serial.println(F("j: wait at min 1"));
  Serial.println(F("k: wait at max 1"));
  Serial.println(F("l: wait at min 2"));
  Serial.println(F("m: wait at max 2"));
  Serial.println(F("n: new factor at max"));
  Serial.println(F("o: offset"));
  Serial.println(F("p: progmem index"));
  Serial.println(F("q: new factor at min"));
  Serial.println(F("r: red LED"));
  Serial.println(F("s: Save current Porperties"));
  Serial.println(F("t: Test all LEDs"));
  Serial.println(F("u: global factor"));
  Serial.println(F("v: Start with current play of light"));
  Serial.println(F("w: Save current properties"));
  Serial.println(F("y: Set number of plays"));
//  Serial.print(ln("y: ");
//  Serial.print(ln("z: ");
  Serial.println();
}

void RgbLedControl::info()
{  
  Serial.println();
  Serial.println(F("number\tcolor\tintensity\tpointer\tpointer_min\tpointer_max\
\tdarker\tdimmable"));

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
  Serial.println(F("PROGMEM_index\tdimFactor\tglobalFactor\tcolorFactor\
\toffset\tduration\tcounter"));

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getProgmemIndex());
      Serial.print("\t\t");
      Serial.print(led[i].getFactor());
      Serial.print("\t\t\t");
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
  Serial.println(F("newFactorAtMin\tnewFactorAtMax\
\tnewMaxPointerAtMin\tnewMinPointerAtMax"));

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
  Serial.println(F("waitAtMin1\twaitAtMax1\twaitAtMin2\twaitAtMax2\tpointerIsChangeable"));

  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getWaitAtMin1());
      Serial.print("\t\t");
      Serial.print(led[i].getWaitAtMax1());
      Serial.print("\t\t");
      Serial.print(led[i].getWaitAtMin2());
      Serial.print("\t\t");
      Serial.print(led[i].getWaitAtMax2());
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
    playOfLight = EEPROM.read(ADDRESS_NUMBER_OF_DEFAULT_PLAY);
    if (playOfLight > MAX_NUMBER_OF_PLAYS)
      {
        playOfLight = DEFAULT_PLAY_OF_LIGHT;
      }
    numberOfPlays = EEPROM.read(ADDRESS_NUMBER_OF_PLAYS);
    if ((numberOfPlays > MAX_NUMBER_OF_PLAYS) || (numberOfPlays == 0))
      {
        numberOfPlays = MAX_NUMBER_OF_PLAYS;
        EEPROM.write(ADDRESS_NUMBER_OF_PLAYS, numberOfPlays);
      }
  }

void RgbLedControl::readEeprom(uint8_t play)
{
  uint8_t globalFactor;
  uint8_t content;
  uint8_t addressStart = 2 + play * (LENGTH_OF_PLAY_PROPERTIES);
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

  globalFactor = EEPROM.read(address + OFFSET_GLOBAL_FACTOR);
  Serial.println(address);
  if ((globalFactor == 0xFF)||(globalFactor == 0x00))
    {
      globalFactor = DEFAULT_GLOBAL_FACTOR;
    }
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].setGlobalFactor(globalFactor);
    }
  Serial.print(F("Global Factor: "));
  Serial.println(globalFactor);
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if ((led[i].getColor() == 'R') || (led[i].getColor() == 'r'))
        {
          address = addressStart + OFFSET_RED;
          Serial.println(F("Red LED:"));
        }
      if ((led[i].getColor() == 'G') || (led[i].getColor() == 'g'))
        {
          address = addressStart + OFFSET_GREEN;
          Serial.println(F("Green LED:"));
        }
      if ((led[i].getColor() == 'B') || (led[i].getColor() == 'b'))
        {
          address = addressStart + OFFSET_BLUE;
          Serial.println(F("Blue LED:"));
        }

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setColorFactor(content);
      Serial.print(F("Color Factor: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setOffset(content);
      Serial.print(F("Offset: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setProgmemIndex(content & 0x0F);
      Serial.print(F("Progmem Index: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setDimmable((bool)content);
      Serial.print(F("Dimmable: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      if (content == 0xFF)
        {
          led[i].setMinPointer(0);
        }
      else
        {
          led[i].setMinPointer(content);
        }
      Serial.print(F("Min. Pointer: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setMaxPointer(content);
      Serial.print(F("Max. Pointer: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setNewFactorAtMin((bool)content);
      Serial.print(F("New Factor at Min:"));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setNewFactorAtMax((bool)content);
      Serial.print(F("New Factor at Max"));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setNewMinPointerAtMax((bool)content);
      Serial.print(F("New min pointer at max: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setNewMaxPointerAtMin((bool)content);
      Serial.print(F("New max pointer at min: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setWaitAtMin1((bool)content);
      Serial.print(F("Wait at min 1: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setWaitAtMax1((bool)content);
      Serial.print(F("Wait at max 1: "));
      Serial.println(content);
      Serial.println();
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setWaitAtMin2((bool)content);
      Serial.print(F("Wait at min 2: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = EEPROM.read(address);
      led[i].setWaitAtMax2((bool)content);
      Serial.print(F("Wait at max 2: "));
      Serial.println(content);
      Serial.println();

      Serial.println();
    }
}

void RgbLedControl::writeEeprom(void)
{
  EEPROM.write(ADDRESS_NUMBER_OF_DEFAULT_PLAY, playOfLight);
}

void RgbLedControl::writeEeprom(uint8_t play)
{
  uint8_t cycleTime;
  uint8_t global_factor;
  uint8_t content;
  uint8_t addressStart = 2 + play * (LENGTH_OF_PLAY_PROPERTIES);
  uint8_t address = addressStart;

  Serial.print(F("Saving LED properties for Play of Light: "));
  Serial.println(play);
  Serial.println();

  Serial.println(address);
  EEPROM.write(address + OFFSET_LOOP_DURATION, (uint8_t)cycleTime);
  Serial.print(F("Time per cycle"));
  Serial.println(cycleTime);
  address ++;

  Serial.println(address);
  content = led[0].getGlobalFactor();
  EEPROM.write(address, content);
  Serial.print(F("Global factor: "));
  Serial.println(content);
  address ++;

  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      if ((led[i].getColor() == 'R') || (led[i].getColor() == 'r'))
        {
          address = addressStart + OFFSET_RED;
          Serial.println(F("Update red LED: "));
        }
      if ((led[i].getColor() == 'G') || (led[i].getColor() == 'g'))
        {
          address = addressStart + OFFSET_GREEN;
          Serial.println(F("Update green LED: "));
        }
      if ((led[i].getColor() == 'B') || (led[i].getColor() == 'b'))
        {
          address = addressStart + OFFSET_BLUE;
          Serial.println(F("Update blue LED: "));
        }

      Serial.println(address);
      content = led[i].getColorFactor();
      EEPROM.write(address, content);
      Serial.print(F("Color Factor: "));
      Serial.println(content);
      address ++;

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
      content = (uint8_t)led[i].getNewFactorAtMin();
      EEPROM.write(address, content);
      Serial.print(F("New Factor at min: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getNewFactorAtMax();
      EEPROM.write(address, content);
      Serial.print(F("New Factor at max: "));
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
      content = (uint8_t)led[i].getWaitAtMin1();
      EEPROM.write(address, content);
      Serial.print(F("Wait at min 1: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getWaitAtMax1();
      EEPROM.write(address, content);
      Serial.print(F("Wait at max 1: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getWaitAtMin2();
      EEPROM.write(address, content);
      Serial.print(F("Wait at min 2: "));
      Serial.println(content);
      address ++;

      Serial.println(address);
      content = (uint8_t)led[i].getWaitAtMax2();
      EEPROM.write(address, content);
      Serial.print("Wait at max 2: ");
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
