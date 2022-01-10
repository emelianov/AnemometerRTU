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
  uint16_t DeviceState;                   // 1
  uint16_t WindDirection;                 // 2
  float    WindSpeed;                     // 3,4
  float    Temperature;                   // 5,6
  float    Humidity;                      // 7,8
  float    AirPressure;                   // 9,10
  uint16_t CompassHeading;                // 11
  uint16_t PrecipitationType;             // 12
  float    PrecipitationIntensity;        // 13,14
  float    AccumulatedPrecepitation;      // 15,16
  uint16_t IntensityUnit;                 // 17
  uint16_t GpsStatus;                     // 18
  float    GpsSpeed;                      // 19,20
  uint16_t GpsHeading;                    // 21
  float    Longitude;                     // 22,23
  float    Latitude;                      // 24,25
  float    DustConcentration;            // 26,27
  float    Visibility;                    // 28,29
  float    RadiationIllimitation;         // 30,31
  float    Reserved;                      // 32,33
  float    RadiationPower;                // 34,35
  uint16_t CompassCorrectedWindDirection; // 36
  float    Altitude;                      // 37,38
} __attribute__((packed));

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
  //for (uint16_t i = 0; i < REG_COUNT; i++) {
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
