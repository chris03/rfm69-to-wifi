#include <Wire.h>
#include <SI7021.h>
#define SDA 0 // Yellow
#define SCL 4 // Orange

namespace LocalSensor
{
const int UPDATE_INTERVAL = 30 * 1000; // 30 sec.
const uint8_t SENSOR_ID = 51;
SI7021 sensor;
bool sensorOk = false;
long nextUpdate = 0;

void setup()
{
    Serial.print("Starting sensor: ");
    sensorOk = sensor.begin(SDA, SCL);
    Serial.println(sensorOk ? "Ok" : "FAIL");
}

void loop()
{
    if (sensorOk && nextUpdate < millis())
    {
        Serial.println("Reading Local Sensor...");

        char buffer[50];
        float temp = sensor.getCelsiusHundredths() / 100.0;
        unsigned int hum = sensor.getHumidityPercent();

        snprintf(buffer, sizeof(buffer), "Temp: %.2f  Hum: %i", temp, hum);
        Serial.println(buffer);

        snprintf(buffer, sizeof(buffer), "idx=%i&nvalue=0&svalue=%.2f;%i", SENSOR_ID, temp, hum);
        Serial.println(buffer);
        Serial.println(httpGet(serverUrl + "json.htm?type=command&param=udevice&" + String(buffer)));

        nextUpdate = millis() + UPDATE_INTERVAL;
    }
}
}
