#include <Arduino.h>

bool isDebugSetup = false;

void setupDebug() {
    Serial.begin(115200);
    isDebugSetup = true;
}

void debug(String s) {
    if (!isDebugSetup)
        setupDebug();
    Serial.println(s);
}

void debug(const char* s) {
    if (!isDebugSetup)
        setupDebug();
    Serial.println(s);
}
