# Writes the intensities to one header file

import math

f = open("intensities.h", "w")
size = 256
max_value = 255
offset = 1
peak2peak = max_value - offset
amplitude = peak2peak / 2

name = "const PROGMEM uint8_t intensities_8bit_" + str(0) + "[] = {";
f.write(name)
expo = math.pi / (2 * size)
for index in range(size):
    value = round(peak2peak * math.cos(index * expo) + offset)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint8_t intensities_8bit_" + str(1) + "[] = {";
f.write(name)
expo = math.pi / size
for index in range(size):
    value = round(amplitude * math.cos(index * expo) + amplitude + offset)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint8_t intensities_8bit_" + str(2) + "[] = {";
f.write(name)
expo = math.pi / (2 * size)
for index in range(size):
    value = round(peak2peak * (math.cos(index * expo))**2 + offset)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint8_t intensities_8bit_" + str(3) + "[] = {";
f.write(name)
expo = peak2peak**(1 / (size - 1))
for index in range(size):
    value = round(peak2peak * expo**(- index)) + offset
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint8_t intensities_8bit_" + str(4) + "[] = {";
f.write(name)
expo = math.log(peak2peak, offset / 2)/(size - 1)
for index in range(size):
    value = round(max_value * math.exp(- index * expo))
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint8_t intensities_8bit_" + str(5) + "[] = {";
f.write(name)
for index in range(size):
    value = round (max_value - index * peak2peak / max_value)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

offset = 1
max_value = 4095
peak2peak = max_value - offset
amplitude = peak2peak / 2

name = "const PROGMEM uint16_t intensities_12bit_" + str(0) + "[] = {";
f.write(name)
expo = math.pi / (2 * size)
for index in range(size):
    value = round(peak2peak * math.cos(index * expo) + offset)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint16_t intensities_12bit_" + str(1) + "[] = {";
f.write(name)
expo = math.pi / size
for index in range(size):
    value = round(amplitude * math.cos(index * expo) + amplitude + offset)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint16_t intensities_12bit_" + str(2) + "[] = {";
f.write(name)
expo = math.pi / (2 * size)
for index in range(size):
    value = round(peak2peak * (math.cos(index * expo))**2 + offset)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint16_t intensities_12bit_" + str(3) + "[] = {";
f.write(name)
expo = math.log(peak2peak, 2)/(size - 1)
for index in range(size):
    value = round(peak2peak * math.exp(-index * expo))
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint16_t intensities_12bit_" + str(4) + "[] = {";
f.write(name)
expo = peak2peak**(1 / (size - 1))
for index in range(size):
    value = round(peak2peak * expo**(- index))
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

name = "const PROGMEM uint16_t intensities_12bit_" + str(5) + "[] = {";
f.write(name)
for index in range(size):
    value = round (max_value - index * 16 * peak2peak / max_value)
    print(index, value)
    f.write(str(value))
    f.write(", ")

f.write("};\n")

f.close()
