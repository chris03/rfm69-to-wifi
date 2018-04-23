#include <Arduino.h>

const int radioLogMaxSize = 4;
byte radioLogIndex = 0;
byte radioLogSize = 0;
String radioLog[radioLogMaxSize];