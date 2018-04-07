#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include "SparkFunCCS811.h"

#define CCS811_ADDR 0x5A

CCS811 mySensor(CCS811_ADDR);

ClosedCube_HDC1080 hdc1080;

void printSerialNumber() {
  Serial.print("  Device Serial Number=");
  HDC1080_SerialNumber sernum = hdc1080.readSerialNumber();
  char format[12];
  sprintf(format, "%02X-%04X-%04X", sernum.serialFirst, sernum.serialMid, sernum.serialLast);
  Serial.println(format);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("CCS811 tVOC/CO2 & HDC1080 temperature/humidity sensor controller");

  hdc1080.begin(0x40);

  Serial.println("HDC1080");
  Serial.print("  Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("  Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
  
  printSerialNumber();

  Serial.println("CCS811");
  //It is recommended to check return status on .begin(), but it is not
  //required.
  CCS811Core::status returnCode = mySensor.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS) {
    Serial.println("  .begin() returned with an error.");
    while (1); //Hang if there was a problem.
  } else {
    Serial.println("connection OK");
  }
}

void loop()
{

  float temperature = hdc1080.readTemperature();
  float humidity = hdc1080.readHumidity();
  Serial.print("CSS811/HDC1080: T=");
  Serial.print(temperature);
  Serial.print("C, RH=");
  Serial.print(humidity);
  Serial.print("%");
  
  //Check to see if data is ready with .dataAvailable()
  if (mySensor.dataAvailable())
  {
    
    mySensor.readAlgorithmResults();

    mySensor.setEnvironmentalData(humidity, temperature);

    Serial.print(", CO2=");
    //Returns calculated CO2 reading
    Serial.print(mySensor.getCO2());
    Serial.print(", tVOC=");
    //Returns calculated TVOC reading
    Serial.print(mySensor.getTVOC());
    Serial.print(", ms=");
    //Simply the time since program start
    Serial.print(millis());
    Serial.print("");
    Serial.println();
  }

  delay(3000);
}

