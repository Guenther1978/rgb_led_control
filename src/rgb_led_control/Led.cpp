#include <Arduino.h>
#include <EEPROM.h>

#include "Led.hpp"


///////////////////////
// get and set methods
///////////////////////



/* offset */

uint8_t Led::getOffset(void)
  {
    return _offset;
  }

void Led::setOffset(uint8_t offset)
  {
    _offset = offset;
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

bool Led::getWaitAtMin(void)
  {
    return _waitAtMin;
  }

void Led::setWaitAtMin(bool waitAtMin)
  {
    _waitAtMin = waitAtMin;
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
      setMaxPointer2Default();
    }
}

void Led::setMaxPointer2Default(void)
{
  _pointerMax = _defaultPointerMax;
}

bool Led::getPointerIsChangeable(void)
  {
    return _pointerIsChangeable;
  }

void Led::setPointerIsChangeable(bool pointerIsChangeable)
  {
    _pointerIsChangeable = pointerIsChangeable;
  }

bool Led::getDimmable(void)
  {
    return _dimmable;
  }

void Led::setDimmable(bool dimmable)
  {
    _dimmable = dimmable;
    if (_dimmable == false)
      {
          _dimm_factor = _defaultFactor;
          _pointerMin = _defaultPointerMin;
          _pointerMax = _defaultPointerMax;
      }
  }

bool Led::getLedIsOn(void)
  {
    return _led_is_on;
  }

void Led::setLedIsOn(bool led_is_on)
  {
    _led_is_on = led_is_on;
    if (!_led_is_on)
      {
        _dimmable = false;
      }
  }


// _factor

uint8_t Led::getFactor()
{
  return _dimm_factor;
}

void Led::setFactor(uint8_t factor)
{
  _dimm_factor = factor;
}

uint8_t Led::getColorFactor()
{
  return _color_factor;
}

void Led::setColorFactor(uint8_t factor)
{
  _color_factor = factor;
}


// _newFactorAtMax

bool Led::getNewFactor()
{
  return _newFactor;
}

void Led::setNewFactor(bool newFactor)
{
  _newFactor = newFactor;
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
  if (_dimmable && _pointerIsChangeable)
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
}

void Led::saveToEeprom(uint8_t address) {
  LedDefaultProperties content;

  if (_dimmable) {
    content.default_booleans |= kBitDimmable;
  }
  else {
    content.default_booleans &= ~kBitDimmable;
  }

  if (_newFactor) {
    content.default_booleans |= kBitNewFactor;
  }
  else {
    content.default_booleans &= ~kBitNewFactor;
  }

  if (_led_is_on) {
    content.default_booleans |= kBitLedIsOn;
  }
  else {
    content.default_booleans &= ~kBitLedIsOn;
  }

  if (_newMinPointerAtMax) {
    content.default_booleans |= kBitNewMinPointerAtMax;
  }
  else {
    content.default_booleans &= ~kBitNewMinPointerAtMax;
  }

  if (_newMaxPointerAtMin) {
    content.default_booleans |= kBitNewMaxPointerAtMin;
  }
  else {
    content.default_booleans &= ~kBitNewMaxPointerAtMin;
  }  if (_waitAtMin) {
    content.default_booleans |= kBitWaitAtMin;
  }
  else {
    content.default_booleans &= ~kBitWaitAtMin;
  }

  if (_waitAtMax) {
    content.default_booleans |= kBitWaitAtMax;
  }
  else {
    content.default_booleans &= ~kBitWaitAtMax;
  }

  content.progmem_index = _progmemIndex;
  content.factor = _color_factor;
  content.offset = _offset;
  content.pointer_min = _defaultPointerMin;
  content.pointer_max = _defaultPointerMax;

  EEPROM.put (address, content);
}

void Led::loadFromEeprom(uint8_t address) {
  LedDefaultProperties content;
  EEPROM.get (address, content);

  _dimmable = content.default_booleans & kBitDimmable;
  _newFactor = content.default_booleans & kBitNewFactor;
  _newMinPointerAtMax = content.default_booleans & kBitNewMinPointerAtMax;
  _newMaxPointerAtMin = content.default_booleans & kBitNewMaxPointerAtMin;
  _waitAtMin = content.default_booleans & kBitWaitAtMin;
  _waitAtMax = content.default_booleans & kBitWaitAtMax;
  _led_is_on = content.default_booleans & kBitLedIsOn;
  _progmemIndex = content.progmem_index;
  _color_factor = content.factor;
  _offset = content.offset;
  _defaultPointerMin = content.pointer_min;
  _defaultPointerMax = content.pointer_max;
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

  product = (uint8_t)(255 - content) * _dimm_factor >> 8;
  sum = (255 -  product) * (255 - _offset) >> 8;
  sum += _offset;
  _intensity = (uint8_t)_color_factor * sum >> 8;
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
  product *= _dimm_factor;
  product = product >> 8;
  sum = 0x0FFF -  product;
  _intensity = (uint16_t)sum * _color_factor;
}
