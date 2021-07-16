#include "Arduino.h"

#define SYSGOOD 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SYSGOOD, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("test");
  delay(500);
  digitalWrite(SYSGOOD, HIGH);
  delay(1000);
  digitalWrite(SYSGOOD, LOW);
  delay(1000);
}