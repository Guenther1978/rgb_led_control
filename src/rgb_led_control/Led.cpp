#include "Led.hpp"

///////////////////////
// get and set methods
///////////////////////

// _number

uint8_t Led::getNumber(void)
{
  return _number;
}

void Led::setNumber(uint8_t number)
{
  _number = number;
}


/* color */

unsigned char Led::getColor(void)
{
  return _color;
}

void Led::setColor(unsigned char color)
{
  _color = color;
}


/* Waiting */

bool Led::getWaitAtMax(void)
  {
    return _waitAtMax;
  }

void Led::setWaitAtMax(bool waitAtMax)
  {
    _waitAtMax = waitAtMax;
  }

void Led::toggleWaitAtMax(void)
{
  _waitAtMax != _waitAtMax;
}

bool Led::getWaitAtMin(void)
  {
    return _waitAtMin;
  }

void Led::setWaitAtMin(bool waitAtMin)
  {
    _waitAtMin = waitAtMin;
  }

void Led::toggleWaitAtMin(void)
{
  _waitAtMin != _waitAtMin;
}


/* progmem_index */

uint8_t Led::getProgmemIndex()
{
  return _progmemIndex;
}

void Led::setProgmemIndex(uint8_t number)
{
  _progmemIndex = number;
}


// _pointer

uint8_t Led::getPointer(void)
{
  return _pointer;
}

void Led::setPointer(uint8_t pointer)
{
  _pointer = pointer;
}

uint8_t Led::getMinPointer(void)
{
  return _pointerMin;
}

void Led::setMinPointer(uint8_t pointerMin)
{
  _pointerMin = pointerMin;
}

bool Led::getNewMinPointerAtMax(void)
{
  return _newMinPointerAtMax;
}

void Led::setNewMinPointerAtMax(bool newMinPointerAtMax)
{
  _newMinPointerAtMax = newMinPointerAtMax;
}

void Led::toggleNewMinPointerAtMax(void)
{
  _newMinPointerAtMax =! _newMinPointerAtMax;
  if (_newMinPointerAtMax == false)
    {
      setMinPointer2Default();
    }
}

void Led::setMinPointer2Default(void)
{
  _pointerMin = _defaultPointerMin;
}

uint8_t Led::getMaxPointer()
{
  return _pointerMax;
}

void Led::setMaxPointer(uint8_t pointerMax)
{
  _pointerMax = pointerMax;
}
bool Led::getNewMaxPointerAtMin(void)
{
  return _newMaxPointerAtMin;
}

void Led::setNewMaxPointerAtMin(bool newMaxPointerAtMin)
{
  _newMaxPointerAtMin = newMaxPointerAtMin;
}

void Led::toggleNewMaxPointerAtMin(void)
{
  _newMaxPointerAtMin =! _newMaxPointerAtMin;
  if (_newMaxPointerAtMin == false)
    {
      _pointerMax = _defaultPointerMax;
    }
}

void Led::setMaxPointer2Default(void)
{
  _pointerMin = _defaultPointerMin;
}



// _factor

uint8_t Led::getFactor()
{
  return _factor;
}

void Led::setFactor(uint8_t factor)
{
  _factor = factor;
}

uint8_t Led::getGlobalFactor()
{
  return _globalFactor;
}

void Led::setGlobalFactor(uint8_t factor)
{
  _globalFactor = factor;
}


// _newFactorAtMax

bool Led::getNewFactorAtMax()
{
  return _newFactorAtMax;
}

void Led::setNewFactorAtMax(bool newFactorAtMax)
{
  _newFactorAtMax = newFactorAtMax;
}

void Led::toggleNewFactorAtMax(void)
{
  _newFactorAtMax =! _newFactorAtMax;
  if (_newFactorAtMax == false)
    {
      _factor = _defaultFactor;
    }
}

bool Led::getNewFactorAtMin()
{
  return _newFactorAtMin;
}

void Led::setNewFactorAtMin(bool newFactorAtMin)
{
  _newFactorAtMin = newFactorAtMin;
}

void Led::toggleNewFactorAtMin(void)
{
  _newFactorAtMin =! _newFactorAtMin;
  if (_newFactorAtMin == false)
    {
      _factor = _defaultFactor;
    }  
}


// _darker

bool Led::getDarker()
{
  return _darker;
}

void Led::setDarker(bool darker)
{
  _darker = darker;
}

void Led::invertDarker()
{
  _darker = !_darker;
}


// _darkerHasChanged, _intensityAtMax, intensityAtMin

bool Led::getDarkerHasChanged()
{
  return _darkerHasChanged;
}

bool Led::getPointerIsAtMin()
{
  return _pointerIsAtMin;
}

bool Led::getPointerIsAtMax()
{
  return _pointerIsAtMax;
}


///////////////////////////////////////////////////////
// methods dealing with the pointer to the intensities
///////////////////////////////////////////////////////

void Led::increasePointer()
{
  _pointer ++;
  if (_pointer == _pointerMax)
    {
      _pointerIsAtMin = false;
      _pointerIsAtMax = true;
      _darker = true;
      _darkerHasChanged = true;
    }
}

void Led::decreasePointer()
{
  _pointer --;
  if (_pointer == _pointerMin)
    {
      _pointerIsAtMin = true;
      _pointerIsAtMax = false;
      _darker = false;
      _darkerHasChanged = true;
    }
}

void Led::changePointer()
{
  _darkerHasChanged = false;
  if (_darker)
    {
      decreasePointer();
    }
  else
    {
      increasePointer();
    }
}


////////////////////////////////////////////////
// methods dealing with the class SpeedControl
////////////////////////////////////////////////

bool Led::letSpeedControlCount()
{
  return speedControl.count();
}

void Led::setSpeedControlDuration(uint8_t duration)
{
  speedControl.setDuration(duration);
}

uint8_t Led::getSpeedControlDuration()
{
  return speedControl.getDuration();
}

void Led::setSpeedControlCounter(uint8_t counter)
{
  speedControl.setCounter(counter);
}

uint8_t Led::getSpeedControlCounter()
{
  return speedControl.getCounter();
}


////////////////////////////////////////////////
// methods of the class Led8bit
////////////////////////////////////////////////


uint8_t Led8bit::getPin(void)
{
  return _pin;
}

void Led8bit::setPin(uint8_t pin)
{
    _pin = pin;
}

void Led8bit::setPin2default(void)
{
  _pin = pwmPins[_number];
}

uint8_t Led8bit::getIntensity(void)
{
  return _intensity;
}

void Led8bit::setIntensity(uint8_t intensity)
{
  _intensity = intensity;
}

void Led8bit::pointer2int()
{
  uint8_t content;
  uint8_t product;
  uint8_t sum;

  switch(_progmemIndex)
    {
    case 0:
      content = pgm_read_word_near(intensities_8bit_0 + _pointer);
      break;
    case 1:
      content = pgm_read_word_near(intensities_8bit_1 + _pointer);
      break;
    case 2:
      content = pgm_read_word_near(intensities_8bit_2 + _pointer);
      break;
    case 3:
      content = pgm_read_word_near(intensities_8bit_3 + _pointer);
      break;
    case 4:
      content = pgm_read_word_near(intensities_8bit_4 + _pointer);
      break;
    case 5:
      content = pgm_read_word_near(intensities_8bit_5 + _pointer);
      break;

    default:
      break;
    }

  product = (uint8_t)(255 - content) * _factor >> 8;
  sum = 255 -  product;
  _intensity = (uint8_t)_globalFactor * sum >> 8;
}

void Led8bit::int2output(void)
{
  analogWrite(_pin, _intensity);
}

////////////////////////////////////////////////
// methods of the class Led16bit
////////////////////////////////////////////////

uint16_t Led16bit::getIntensity(void)
{
  return _intensity;
}

void Led16bit::setIntensity(uint16_t intensity)
{
  _intensity = intensity;
}

void Led16bit::pointer2int()
{
  uint16_t content;
  uint32_t product;
  uint16_t sum;

  switch(_progmemIndex)
    {
    case 0:
      content = pgm_read_word_near(intensities_12bit_0 + _pointer);
      break;
    case 1:
      content = pgm_read_word_near(intensities_12bit_1 + _pointer);
      break;
    case 2:
      content = pgm_read_word_near(intensities_12bit_2 + _pointer);
      break;
    case 3:
      content = pgm_read_word_near(intensities_12bit_3 + _pointer);
      break;
    case 4:
      content = pgm_read_word_near(intensities_12bit_4 + _pointer);
      break;
    case 5:
      content = pgm_read_word_near(intensities_12bit_5 + _pointer);
      break;

    default:
      break;
    }

  product = (0x0FFF - content);
  sum = 0x0FFF -  product;
  _intensity = (uint16_t)sum;
}
