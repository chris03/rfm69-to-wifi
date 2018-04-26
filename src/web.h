#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

void setupWebServer()
{
    // Web server
    server.on("/", []() {
        String temperature = LocalSensor::sensorOk ? String(LocalSensor::sensor.getCelsiusHundredths() / 100) : "?";
        String humidity = LocalSensor::sensorOk ? String(LocalSensor::sensor.getHumidityPercent()) : "?";

        server.send(200, "text/plain", "ESP8266\nRadio: " + String(radioInit) + "\nSensor: " + String(LocalSensor::sensorOk) + "\nVCC: " + String(ESP.getVcc()) + "\nFreeHeap: " + String(ESP.getFreeHeap()) + F("\n\nTemp: ") + temperature + F(" Hum: ") + humidity + "\n\nRadio Log: " + RadioLog::get());
    });
    server.on("/radio", []() {
        String t = String(radio.readTemperature(-1));
        server.send(200, "text/plain", "Temp is: " + t);
    });

    server.onNotFound([]() {
        server.send(404, "text/plain", "Not Found");
    });

    server.begin();
    Serial.println("HTTP server started");
}