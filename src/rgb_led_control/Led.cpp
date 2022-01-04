#include "Led.hpp"

///////////////////////
// get and set methods
///////////////////////

// _number

uint8_t Led::getNumber()
{
  return _number;
}

void Led::setNumber(uint8_t number)
{
  _number = number;
}


// _pointer

uint8_t Led::getPointer()
{
  return _pointer;
}

void Led::setPointer(uint8_t pointer)
{
  _pointer = pointer;
}

uint8_t Led::getMinPointer()
{
  return _pointerMin;
}

void Led::setMinPointer(uint8_t pointerMin)
{
  _pointerMin = pointerMin;
}

uint8_t Led::getMaxPointer()
{
  return _pointerMax;
}

void Led::setMaxPointer(uint8_t pointerMax)
{
  _pointerMax = pointerMax;
}

uint8_t Led::getProgmemIndex()
{
  return _progmemIndex;
}

void Led::setProgmemIndex(uint8_t number)
{
  _progmemIndex = number;
}


// new Pointer

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
      _pointerMin = _defaultPointerMin;
    }
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

bool Led::getIntensityOrPointerAtMin()
{
  return _intensityOrPointerAtMin;
}

bool Led::getIntensityOrPointerAtMax()
{
  return _intensityOrPointerAtMax;
}


//////////////////////////////////////
// methods dealing with the intensity
//////////////////////////////////////

void Led::increaseIntensity()
{
  _intensity ++;
  if (_intensity == _intensityMax)
    {
      _intensityOrPointerAtMin = false;
      _intensityOrPointerAtMax = true;
      _darker = true;
      _darkerHasChanged = true;
    }
}

void Led::decreaseIntensity()
{
  _intensity --;
  if (_intensity == _intensityMin)
    {
      _intensityOrPointerAtMin = true;
      _intensityOrPointerAtMax = false;
      _darker = true;
      _darkerHasChanged = true;
    }
}

void Led::changeIntensity()
{
  _darkerHasChanged = false;
  if (_darker)
    {
      decreaseIntensity();
    }
  else
    {
      increaseIntensity();
    }
}


///////////////////////////////////////////////////////
// methods dealing with the pointer to the intensities
///////////////////////////////////////////////////////

void Led::increasePointer()
{
  _pointer ++;
  if (_pointer == _pointerMax)
    {
      _intensityOrPointerAtMin = false;
      _intensityOrPointerAtMax = true;
      _darker = true;
      _darkerHasChanged = true;
    }
}

void Led::decreasePointer()
{
  _pointer --;
  if (_pointer == 0)
    {
      _intensityOrPointerAtMin = true;
      _intensityOrPointerAtMax = false;      
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

void Led::pointer2int()
{
  uint8_t content;
  uint8_t product;
  uint8_t sum;
  
  switch(_progmemIndex)
    {
    case 0:
      content = pgm_read_word_near(intensities_0 + _pointer);
      break;
    case 1:
      content = pgm_read_word_near(intensities_1 + _pointer);
      break;
    case 2:
      content = pgm_read_word_near(intensities_2 + _pointer);
      break;
    case 3:
      content = pgm_read_word_near(intensities_3 + _pointer);
      break;
    case 4:
      content = pgm_read_word_near(intensities_4 + _pointer);
      break;

    default:
      break;
    }
  
  product = (uint8_t)(255 - content) * _factor >> 8;
  sum = 255 -  product;
  _intensity = (uint8_t)_globalFactor * sum >> 8;
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
