#include <Arduino.h>

namespace RadioLog
{
const int radioLogMaxSize = 4;
byte radioLogIndex = 0;
byte radioLogSize = 0;
String radioLog[radioLogMaxSize];

void add(String data)
{
    if (radioLogSize < radioLogMaxSize)
    {
        radioLogSize++;
    }
    radioLog[radioLogIndex] = data;
    radioLogIndex = (radioLogIndex + 1) % radioLogMaxSize;
}

String get()
{
    String t = "";
    byte index = radioLogIndex == 0 ? radioLogSize - 1 : radioLogIndex - 1;

    for (byte i = 0; i < radioLogSize; ++i)
    {
        t += "\n" + radioLog[index];
        index = index == 0 ? radioLogSize - 1 : index - 1;
    }

    return t;
}
}