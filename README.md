# rfm69-to-wifi

## Purpose
* Receive data on the RFM69 radio and POST it to a remote server via Wifi.
* Accept OTA (over the air) updates.

## Hardware
ESP-12  
RFM69W 915Mhz radio  

## Wiring
```
RFM69     ESP12
---------------------
DIO0  <=> GPIO5 (Configurable in code)
NSS   <=> GPIO15 (CS)
CLK   <=> GPIO14
MOSI  <=> GPIO13
MISO  <=> GPIO12
```

## External Libraries
https://github.com/LowPowerLab/RFM69  
https://github.com/LowPowerLab/SI7021  
