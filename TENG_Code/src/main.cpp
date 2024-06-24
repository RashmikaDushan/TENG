#include <Arduino.h>
#include <CyclicBuffer.h>

#define sensorPin 36
#define bufferSize 10

CyclicBuffer buffer(bufferSize);
uint8_t reading = 0;
uint8_t byteArray[bufferSize * sizeof(uint8_t)];

void setup() {
    Serial.begin(9600);
    pinMode(sensorPin, INPUT);
    analogReadResolution(8);
}

void loop() {
    Serial.print("Reading: ");
    reading = analogRead(sensorPin);
    Serial.print(reading);
    Serial.print("          ");
    buffer.push(reading);
    buffer.print(false);
    Serial.print("          ");
    buffer.toByteArray(byteArray);
    for (int i = bufferSize-1; i >= 0; i--) {
        Serial.printf("%02X", byteArray[i]);
        Serial.print(" ");
    }
    Serial.print("          ");
    buffer.printHex(true);
    delay(500);
}