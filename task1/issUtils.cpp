#include "issUtils.h"
#include <Arduino.h>

void log(const char* label, float value) {
    Serial.print(label);
    Serial.println(value);
}

void log(const char* text) {
    Serial.println(text);
}
