//
// Author:
//   Chris03
//
// Purpose:
//   - Receive data on the RFM69 radio
//     and POST it to a remote server via Wifi.
//   - Accept OTA (over the air) updates.
//
// Hardware:
//   ESP-12
//   RFM69W 915Mhz radio
//
// Libraries used:
//   https://github.com/LowPowerLab/RFM69
//   https://github.com/LowPowerLab/SI7021
//
#include <Arduino.h>
#include "httpClient.h"
#include "wifi.h"
#include "ota.h"
#include "localSensor.h"
#include "remoteSensor.h"
#include "radio.h"
#include "radioLog.h"
#include "web.h"

ADC_MODE(ADC_VCC);

void setup(void)
{
  Serial.begin(115200);
  Serial.println();

  setupWifi();
  setupOta();
  LocalSensor::setup();
  setupRadio();
  setupWebServer();
}

void loop(void)
{
  ArduinoOTA.handle();

  server.handleClient();

  LocalSensor::loop();

  if (radioInit && radio.receiveDone())
  {
    Serial.println("");
    Serial.println("----Receive Done----");

    String data((char *)radio.DATA);
    String senderId = String(radio.SENDERID);
    String rssi = String(radio.RSSI);
    data += "(ID|" + senderId + ")";
    data += "(RSSI|" + rssi + ")";

    RadioLog::add(data);

    Serial.println(data);

    parseReceivedData(radio.SENDERID, (char *)radio.DATA, serverUrl);
  }
}
