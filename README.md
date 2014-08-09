This is the Adafruit MAX31885 Arduino Library ported to Spark Core
---

### Added hardware SPI and calibration
For hardware SPI only need to specify chip select pin. Otherwise specify 3 pins (as below)

Can calibrate to internal temp sensor, or an arbitrary calibration (note that the function is fed the _calibration_ value, not the current temperature value.  See the example for a possible method)

Uses a 4-sample moving average, via multiplication and bitshifting. Can adjust by changing a couple of lines in the source file (see comments in the read Celsius function)

---

Tested and works great with the Adafruit Thermocouple Breakout w/MAX31885K http://www.adafruit.com/products/269

These modules use SPI to communicate, 3 pins are required to
interface. Technically this is a software SPI implementation, so these pins can be defined anywhere on the Spark Core. For simplicity, the pins used below are the same pins used for the Hardware SPI.

```
MAX31885 Breakout    Spark Core
Vin                  3V3*
3Vo                  (No Connection)
GND                  GND
DO (Data Out)        A4 (MISO)
CS (Chip Select)     A2 (SS)
CLK (Clock)          A3 (SCK)
```

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries. Ported to Spark Core by Technobly. Hardware and calibration added by Mumblepins.
BSD license, check LICENSE for more information All text above must be included in any redistribution
