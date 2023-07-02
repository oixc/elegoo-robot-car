#include "motor.h"

Motor AppMotor;

void setup() {
  AppMotor.init();
  delay(2000);
  int speed = 100;
  AppMotor.control(true, speed, true, speed / 2);
  delay(2000);
  AppMotor.stop();
}

void loop() {
}
