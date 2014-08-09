/*************************************************** 
  This is a library for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


/* **********************************************
 *  Ported to Spark initially by Technobly
 *  Modified to hardware SPI by Mumblepins and calibration added
 *  Mumblepins copied heavily from Technobly's SD card library,
 *  so most credit goes to him :-)
 **************************************************/

#include "application.h"

class Adafruit_MAX31855 {
public:
    Adafruit_MAX31855(int8_t cs_pin, double calibration);
    Adafruit_MAX31855(int8_t cs_pin);

    double readInternal(void);
    double readCelsius(bool raw = false);
    double readFarenheit(void);
    uint8_t readError();
    void calibrate(void);
    void calibrate(double calibration);
    double readCalibration(void);

private:
    int8_t _cs;
    double _calibration;
    uint32_t spiread32(void);
    uint32_t _movingRawTemp;
    //uint8_t _spimode; //0 is software, 1 is hardware
    void chipSelectHigh(void);
    void chipSelectLow(void);
};
