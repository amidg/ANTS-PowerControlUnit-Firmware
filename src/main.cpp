#include "Arduino.h"

#define SYSGOOD 13
#define ENABLE12V 14 
#define ENABLE5VD 15
#define ENABLE3V3D 16
#define STARTCHARGING 17
#define DETECTCHARGING 26
#define SCL 22
#define SDA 21

//PCU FUNCTIONS:
void startCharging(bool isChargingDetected);
void enabledRouterSpeakerPower(void);
void enabled5VdigitalPower(void);
void enable3V3digitalPower(void);

//main functions
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SYSGOOD, OUTPUT);
  pinMode(ENABLE12V, OUTPUT);
  pinMode(ENABLE3V3D, OUTPUT);
  pinMode(ENABLE5VD, OUTPUT);
  pinMode(STARTCHARGING, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("start charging");

}

//functions
void startCharging(bool isChargingDetected) {
  if (isChargingDetected) {
    digitalWrite(STARTCHARGING, HIGH);
    Serial.println("charging started");
    delay(2000);
  } else {
    Serial.println("ERROR: CHARGING NOT DETECTED");
  }

  digitalWrite(STARTCHARGING, LOW);
  delay(2000);
}