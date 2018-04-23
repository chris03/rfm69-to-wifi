#include <Wire.h>
#include <SI7021.h>
#define SDA 0 // Yellow
#define SCL 4 // Orange
SI7021 sensor;
bool sensorInit;

void setupSensor()
{
    Serial.print("Starting sensor: ");
    sensorInit = sensor.begin(SDA, SCL);
    Serial.println(sensorInit ? "Ok" : "FAIL");
}