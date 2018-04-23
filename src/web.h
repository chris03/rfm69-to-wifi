#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

ESP8266WebServer server(80);

void setupWebServer()
{
    // Web server
    server.on("/", []() {
        String t = "";
        byte index = radioLogIndex == 0 ? radioLogSize - 1 : radioLogIndex - 1;
        for (byte i = 0; i < radioLogSize; ++i)
        {
            t += "\n" + radioLog[index];
            index = index == 0 ? radioLogSize - 1 : index - 1;
        }

        String temperature = sensorInit ? String(sensor.getCelsiusHundredths() / 100) : "?";
        String humidity = sensorInit ? String(sensor.getHumidityPercent()) : "?";

        server.send(200, "text/plain", "ESP8266\nRadio: " + String(radioInit) + "\nSensor: " + String(sensorInit) + "\nVCC: " + String(ESP.getVcc()) + "\nFreeHeap: " + String(ESP.getFreeHeap()) + F("\n\nTemp: ") + temperature + F(" Hum: ") + humidity + "\n\nRadio Log: " + t);
    });
    server.on("/radio", []() {
        String t = String(radio.readTemperature(-1));
        server.send(200, "text/plain", "Temp is: " + t);
    });

    server.on("/temp", []() {
        String temperature = sensorInit ? String(sensor.getCelsiusHundredths() / 100) : "?";
        String humidity = sensorInit ? String(sensor.getHumidityPercent()) : "?";
        server.send(200, "text/plain", String() + F("{\"temp\": ") + temperature + F(", \"hum\": ") + humidity + F("}"));
    });

    server.onNotFound([]() {
        server.send(404, "text/plain", "Not Found");
    });

    server.begin();
    Serial.println("HTTP server started");
}

void httpPost(String data, String url)
{
  HTTPClient http;
  if (!http.begin(url))
  {
    Serial.println("http.begin failed");
  }
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  Serial.print("http.POST: ");
  int code = http.POST("data=" + data);
  Serial.println(code);
    Serial.print("Starting radio: ");
  if (code < 0)
  {
    Serial.println(http.errorToString(code));
  }
  http.writeToStream(&Serial);
  Serial.println("");
  http.end();
}