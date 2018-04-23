#include <RFM69.h>
#define NODEID 1
#define NETWORKID 100
#define FREQUENCY RF69_915MHZ

// DIO0 GPIO5 (configurable, see below)
// NSS  GPIO15 (CS)
// CLK  GPIO14
// MOSI GPIO13
// MISO GPIO12
#define RADIO_INTERRUPT 5 // DIO0
RFM69 radio(15, RADIO_INTERRUPT, false, RADIO_INTERRUPT);
bool radioInit;

void setupRadio()
{
    // Reset radio (GPIO4 = RST)
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    delayMicroseconds(100);
    digitalWrite(4, LOW);
    delay(5);

    // Radio
    Serial.print("Starting radio: ");
    radioInit = radio.initialize(FREQUENCY, NODEID, NETWORKID);
    Serial.println(radioInit ? "Ok" : "FAIL");

    // RFM69HW/HCW!
    radio.setHighPower();

    // radio.promiscuous(true);
}