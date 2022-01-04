#include "SpeedControl.hpp"

uint8_t SpeedControl::getCounter()
{
  return _counter;
}

void SpeedControl::setCounter(uint8_t counter)
{
  _counter = counter;
}

uint8_t SpeedControl::getNumber()
{
  return _number;
}

void SpeedControl::setNumber(uint8_t number)
{
  _number = number;
}

uint8_t SpeedControl::getDuration(void)
{
  return _duration;
}

void SpeedControl::setDuration(uint8_t duration)
{
  _duration = duration;
}

bool SpeedControl::count()
{
  bool counterEqualZero = false;
  _counter --;
  if (_counter <= 0)
    {
      _counter = _duration;
      counterEqualZero = true;
    }
  return counterEqualZero;
}
