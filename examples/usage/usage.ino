// Copyright (c) 2019 Jidesh Veeramachaneni
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).

#include "MH-Z19B-CO2-Sensor-Serial.h"

MHZ19BCO2SensorSerial<USARTSerial> mhz19b(Serial1);

void setup() {
  Serial.begin();
}

unsigned long warm_up_time = 180;
unsigned long last = 0;

void loop() {
  unsigned long now = millis();
  if ((now - last) > 10000) {
    if (warm_up_time > 0) {
      warm_up_time -= 10;
      Serial.println("Warming Up..." + String(warm_up_time));
    } else {
      int co2 = mhz19b.Read();
      Serial.println("CO2 Reading: " + String(co2,DEC));
    }
    last = now;
  }
}
