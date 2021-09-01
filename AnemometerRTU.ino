/*
  Modbus Library for Arduino Example - Modbus RTU Anemometer + Custom ESP32 board Client
  
  (c)2021 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266
*/

#include <ModbusRTU.h>

#define SLAVE_ID 1
#define TXD_PIN   35         
#define RXD_PIN   32
#define RE_PIN 33
#define DE_PIN 34
#define REG_BASE 0
#define REG_COUNT 38

struct dataset_t {
  uint16_t DeviceState;                   // 0
  uint16_t WindDirection;                 // 1
  float    WindSpeed;                     // 2,3
  float    Temperature;                   // 4,5
  float    Humidity;                      // 6,7
  float    AirPressure;                   // 8,9
  uint16_t CompassHeading;                // 10
  uint16_t PrecipitationType;             // 11
  float    PrecipitationIntensity;        // 12,13
  float    AccumulatedPrecepitation;      // 14,15
  uint16_t IntensityUnit;                 // 16
  uint16_t GpsStatus;                     // 17
  float    GpsSpeed;                      // 18,19
  uint16_t GpsHeading;                    // 20
  float    Longitude;                     // 21,22
  float    Latitude;                      // 23,24
  float    DiustConcentration;            // 25,26
  float    Visibility;                    // 27,28
  float    RadiationIllimitation;         // 29,30
  float    Reserved;                      // 31,32
  float    RadiationPower;                // 33,34
  uint16_t CompassCorrectedWindDirection; // 35,36
  float    Altitude;                      // 37,38
};

ModbusRTU mb;
dataset_t dataset;

bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data) { // Callback to monitor errors and process results
  if (event != Modbus::EX_SUCCESS) { // Check for error
    Serial.print("Request result: 0x");
    Serial.print(event, HEX);
    return false;
  }
  // Uncomment lines below if no modbus errors but data values looks incorrect
  //uint16_t* reg_array = (uint16_t*)&dataset;
  //for (uint16_t i = 0; i < (sizeof(dataset_t) / 2); i++) {
  //  reg_array[i] = __swap_16(reg_array[i]);
  //}

  // Put your data processing or printing code here.
  // Just note that it should not be too complex
  Serial.print("Temperature: ");
  Serial.println(dataset.Temperature);
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8E1, RXD_PIN, TXD_PIN);  // Initialize Serial1 port on ESP32 pins according to schematics
                                                      // and Speed and format according to the device manual 
  mb.begin(&Serial1, DE_PIN, RE_PIN, true);           // Assign RE and DE pins for data flow control
  mb.master();
}

void loop() {
  if (!mb.slave()) {    // Check if no request in progress
    mb.readHreg(SLAVE_ID, REG_BASE, (uint16_t*)&dataset, REG_COUNT, cb); // Send request to read 38 Holding registers from the device
    while(mb.slave()) { // Wait for responce
      mb.task();
      delay(10);
    }
  }
  delay(5000);
}
