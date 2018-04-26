#include <Arduino.h>
#include <Regexp.h>

struct SensorConfig
{
    String name;
    uint8_t id;
    uint8_t tempId;
    uint8_t voltId;
    bool hasBarometer;
};

std::vector<SensorConfig> remoteSensors =
    {{"Chambre", 10, 38, 44, false},
     {"Salon", 11, 39, 42, true}};

void parseReceivedData(const uint8_t senderId, const char *data, const String &serverUrl)
{
    Serial.print("Id: ");
    Serial.println(senderId);

    // Find sensor config by id
    std::vector<SensorConfig>::iterator it = std::find_if(remoteSensors.begin(), remoteSensors.end(), [&](const SensorConfig &c) -> bool { return c.id == senderId; });

    if (it == end(remoteSensors))
    {
        Serial.println("No config for sensor #" + senderId);
    }
    else
    {
        char toSend[50];
        SensorConfig s = *it;
        Serial.println(s.name);

        // Init regex matcher
        MatchState ms;
        ms.Target((char *)data);

        // Match temperature & humidity
        if (ms.Match("%(SI|(-?%d+%.%d+)|(%d+)%)") == REGEXP_MATCHED)
        {
            char temp[8];
            char hum[4];
            const char *baro = ";0;0";

            ms.GetCapture(temp, 0);
            ms.GetCapture(hum, 1);

            // Print to console
            snprintf(toSend, sizeof(toSend), "Temp: %s  Hum: %s", temp, hum);
            Serial.println(toSend);

            // Format output
            snprintf(toSend, sizeof(toSend), "idx=%i&nvalue=0&svalue=%s;%s;0%s", s.tempId, temp, hum, s.hasBarometer ? baro : "");
            Serial.println(toSend);

            // Send to server
            Serial.println(httpGet(serverUrl + "json.htm?type=command&param=udevice&" + String(toSend)));
        }

        // Match voltage
        if (ms.Match("%(V|(%d+%.%d+)%)") == REGEXP_MATCHED)
        {
            char volt[6];
            ms.GetCapture(volt, 0);

            // Print to console
            snprintf(toSend, sizeof(toSend), "Voltage: %s", volt);
            Serial.println(toSend);

            // Format output
            snprintf(toSend, sizeof(toSend), "idx=%i&nvalue=0&svalue=%s", s.voltId, volt);
            Serial.println(toSend);

            // Send to server
            Serial.println(httpGet(serverUrl + "json.htm?type=command&param=udevice&" + String(toSend)));
        }
    }
}
