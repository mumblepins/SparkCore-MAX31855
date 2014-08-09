/*************************************************** 
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

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

#include "math.h"
#include "adafruit-max31855/adafruit-max31855.h"

int thermoCLK = A3;
int thermoCS = A2;
int thermoDO = A4;

AdafruitMAX31855 thermocouple(thermoCS); //hardware mode
//AdafruitMAX31855 thermocouple(thermoCLK,thermoCS,thermoD0);  //software mode

void setup() {
    //init thermocouple
    thermocouple.init();
    // open serial terminal and press ENTER to start
    Serial.begin(9600);
    while (!Serial.available()) SPARK_WLAN_Loop();
    while (Serial.available()) Serial.read(); //flush serial data
    Serial.println("MAX31855 test");
    // wait for MAX chip to stabilize
    delay(500);
}

void loop() {
    // basic readout test, just print the current temp
    Serial.print("Internal Temp = ");
    Serial.println(thermocouple.readInternal());
    Serial.print("Calibration Value = ");
    Serial.println(thermocouple.readCalibration());
    double c = thermocouple.readCelsius();
    if (isnan(c)) {
        Serial.println("Something wrong with thermocouple!");
    } else {
        Serial.print("C = ");
        Serial.println(c);
    }
    //Serial.print("F = ");
    //Serial.println(thermocouple.readFarenheit());

    delay(1000);
    //press any key to calibrate to internal temp sensor
    if (Serial.available()) thermocouple.calibrate();
    while (Serial.available()) Serial.read(); //flush serial data
}
