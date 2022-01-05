#include "RgbLedControl.hpp"

SoftwareSerial bluetoothCommunicator(2, 3);
  
void RgbLedControl::setup()
{
  uint8_t index;
  
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i ++)
  {
    led[i].setNumber(i);
    led[i].setFactor(0xFF);
    led[i].setGlobalFactor(0xFF);
    led[i].setNewFactorAtMin(true);
    led[i].setNewFactorAtMax(false);
    led[i].setNewMaxPointerAtMin(false);
    led[i].setNewMinPointerAtMax(false);
    led[i].setProgmemIndex(1);
    led[i].setPin(pwm_pins[i]);
  }

  oldMillis = millis();
  outputOfLoopDuration = false;

  randomSeed(analogRead(0));

  readEeprom();
  
  bluetoothCommunicator.begin(9600);
  
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Setup completed");
  Serial.println();
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
          }
      }
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
    }
  }
  loopDuration = millis() - oldMillis;
  if (outputOfLoopDuration)
    {
      Serial.print(loopDuration);
      Serial.print(", ");
    }
  while (millis() < (oldMillis + cycleTime));
  oldMillis = millis();
  if (Serial.available())
    {
      incomingByte = Serial.read();
      switch (incomingByte)
        {
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
          testAllLeds();
          break;
        case 'm':
        case 'M':
          changeLoopDuration(false);
          break;
        case 'n':
        case 'N':
          setGlobalFactor(false);
          break;
        case 'o':
        case 'O':
          setIndex(false);
          break;
        case 'p':
        case 'P':
          toggleNewFactor();
          break;
        case 'q':
        case 'Q':
          toggleNewPointer();
          break;
        case 'r':
        case 'R':
          writeEeprom();
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
        case 'm':
        case 'M':
          changeLoopDuration(true);
          break;
        case 'n':
        case 'N':
          setGlobalFactor(true);
          break;
        case 'o':
        case 'O':
          setIndex(true);
          break;
        default:
          break;
        }
    }
}

void RgbLedControl::help()
{
  Serial.println();
  Serial.println("h: Help");
  Serial.println("i: Info");
  Serial.println("j: Test all LEDs");
  Serial.println("m: Change the duration of the loop");
  Serial.println("n: Set a new global factor");
  Serial.println("o: Set a new intensity array");
  Serial.println("p: Toggles if a new factor should be set");
  Serial.println("q: Toggles if a new pointer shoud be set");
  Serial.println("r: Save the cycle time and the PROGMEM index");
  Serial.println();
}

void RgbLedControl::info()
{  
  Serial.println();
  Serial.println("number\tintensity\tpointer\tFactor\tglobalFactor\tPROGMEM_index\tdarker\tduration\tcounter\tnewFactor\tnewPointer");
  for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
      Serial.print(led[i].getNumber());
      Serial.print("\t");
      Serial.print(led[i].getIntensity());
      Serial.print("\t\t");
      Serial.print(led[i].getPointer());
      Serial.print("\t");
      Serial.print(led[i].getFactor());
      Serial.print("\t");
      Serial.print(led[i].getGlobalFactor());
      Serial.print("\t\t");
      Serial.print(led[i].getProgmemIndex());
      Serial.print("\t\t");
      Serial.print(led[i].getDarker());
      Serial.print("\t");
      Serial.print(led[i].getSpeedControlDuration());
      Serial.print("\t\t");
      Serial.print(led[i].getSpeedControlCounter());
      Serial.print("\t");
      Serial.print(led[i].getNewFactorAtMin());
      Serial.print("\t\t");
      Serial.println(led[i].getNewMaxPointerAtMin());
    }
  Serial.println();
  Serial.print("loop duration / ms:\t");
  Serial.println(loopDuration);
  Serial.print("cycle time / ms:\t");
  Serial.println(cycleTime);
  Serial.println();
}

int8_t RgbLedControl::getNumber()
{
  byte incomingNumber = 'g';
  int8_t number = -2;

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
        default:
          number = -2;
          break;
        }
    }while (number <= -2);
    
  Serial.println(number);
  return number;
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
        newGlobalFactor = 95 + incomingByte * 40;
  
        for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
        {
          led[i].setGlobalFactor(newGlobalFactor);
        }
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

void RgbLedControl::toggleNewFactor(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleNewFactorAtMin();
    }
}

 void RgbLedControl::toggleNewPointer(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      led[i].toggleNewMaxPointerAtMin();
    }
}

void RgbLedControl::readEeprom()
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
