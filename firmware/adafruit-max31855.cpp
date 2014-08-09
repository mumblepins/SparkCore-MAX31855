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

#include "math.h"
#include "adafruit-max31855.h"

AdafruitMAX31855::AdafruitMAX31855(int8_t sclk_pin, int8_t cs_pin, int8_t miso_pin) {
    //software mode without calibration value
    _sclk = sclk_pin;
    _cs = cs_pin;
    _miso = miso_pin;
    _calibration = 0.0;
    _spimode = 0;
}

AdafruitMAX31855::AdafruitMAX31855(int8_t sclk_pin, int8_t cs_pin, int8_t miso_pin, double calibration) {
    //software mode with calibration value
    _sclk = sclk_pin;
    _cs = cs_pin;
    _miso = miso_pin;
    _calibration = calibration;
    _spimode = 0;
}

AdafruitMAX31855::AdafruitMAX31855(int8_t cs_pin) {
    //hardware mode without calibration value
    _cs = cs_pin;
    _calibration = 0.0;
    _spimode = 1;
}

AdafruitMAX31855::AdafruitMAX31855(int8_t cs_pin, double calibration) {
    //hardware mode with calibration value
    _cs = cs_pin;
    _calibration = calibration;
}

inline void AdafruitMAX31855::chipSelectHigh(void) {
    digitalWrite(_cs, HIGH);
}

inline void AdafruitMAX31855::chipSelectLow(void) {
    digitalWrite(_cs, LOW);
}

int AdafruitMAX31855::init(void) {
    if (_spimode) { //hardware mode
        pinMode(_cs, OUTPUT);
        SPI.setClockDivider(SPI_CLOCK_DIV8);
        SPI.setDataMode(SPI_MODE0);
        SPI.setBitOrder(MSBFIRST);
        SPI.begin(_cs);
    } else { //software mode
        pinMode(_cs, OUTPUT);
        pinMode(_sclk, OUTPUT);
        pinMode(_miso, INPUT);
    }
    chipSelectHigh();

    // now we need to do an initial value on the moving average. 
    // this also confirms that thermocouple is attached and working
    int32_t v;
    v = spiread32();
    if (v & 0x7) {
        // uh oh, a serious problem!
        return NAN;
    }
    if (v & 0x80000000) {
        // Negative value, drop the lower 18 bits and explicitly extend sign bits.
        v = 0xFFFFC000 | ((v >> 18) & 0x00003FFFF);
    } else {
        // Positive value, just drop the lower 18 bits.
        v >>= 18;
    }
    v <<= 2; // add an extra two bits for more accuracy
    v += 8741; //convert to Kelvin
    _movingRawTemp = v;
    chipSelectHigh();
    return _movingRawTemp;
}

double AdafruitMAX31855::readInternal(void) {
    uint32_t v;
    v = spiread32();

    // ignore bottom 4 bits - they're just thermocouple data
    v >>= 4;

    // pull the bottom 11 bits off
    float internal = v & 0x7FF;
    internal *= 0.0625; // LSB = 0.0625 degrees
    // check sign bit!
    if (v & 0x800)
        internal *= -1;
    //Serial.print("\tInternal Temp: "); Serial.println(internal);
    return internal;
}

double AdafruitMAX31855::readCelsius(bool raw) {
    int32_t v;

    v = spiread32();

    if (v & 0x7) {
        // uh oh, a serious problem!
        return NAN;
    }

    if (v & 0x80000000) {
        // Negative value, drop the lower 18 bits and explicitly extend sign bits.
        v = 0xFFFFC000 | ((v >> 18) & 0x00003FFFF);
    } else {
        // Positive value, just drop the lower 18 bits.
        v >>= 18;
    }

    v <<= 2; // add an extra two bits for more accuracy
    v += 8741; //convert to Kelvin

    //_movingRawTemp = ((_movingRawTemp * 7)+(uint32_t) v) >> 3;  //8-sample moving average
    _movingRawTemp = ((_movingRawTemp * 3)+(uint32_t) v) >> 2; //4-sample moving average
    //_movingRawTemp = v;  //no moving average

    double celsius = (double) (_movingRawTemp - 8741) / 16.0;

    if (!raw)
        celsius += _calibration;

    return celsius;
}

uint8_t AdafruitMAX31855::readError() {
    return spiread32() & 0x7;
}

double AdafruitMAX31855::readFarenheit(void) {
    float f = readCelsius();
    f *= 9.0;
    f /= 5.0;
    f += 32;
    return f;
}

void AdafruitMAX31855::calibrate(void) {
    double thermoTemp = readCelsius(true);
    double intTemp = readInternal();
    _calibration = intTemp - thermoTemp;
}

void AdafruitMAX31855::calibrate(double calibration) {
    _calibration = calibration;
}

double AdafruitMAX31855::readCalibration(void) {
    return _calibration;
}

uint32_t AdafruitMAX31855::spiread32(void) {
    uint32_t d = 0;
    int i;
    if (_spimode) { //hardware
        chipSelectLow(); // select card
        for (i = 0; i < 4; i++) { //transfer 4 bytes
            d <<= 8;
            d |= SPI.transfer(0XFF);
        }
        chipSelectHigh();
    } else {  // bit-banging
        digitalWrite(_sclk, LOW);
        delayMicroseconds(10);
        chipSelectLow();
        delayMicroseconds(10);

        for (i = 31; i >= 0; i--) {
            digitalWrite(_sclk, LOW);
            delayMicroseconds(10);
            d <<= 1;
            if (digitalRead(_miso)) {
                d |= 1;
            }

            digitalWrite(_sclk, HIGH);
            delayMicroseconds(10);
        }

        chipSelectHigh();
    }
    return d;
}
