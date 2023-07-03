#include "motor.h"

Motor AppMotor;

int speed = 100;
char c;

void setup() {
  AppMotor.init();
  delay(1000);
  
  Serial.begin(9600);  // Default communication rate of the Bluetooth module
  //Serial.println("ready");
}

void turn(bool left, int speed) {
  if (left) {
    AppMotor.control(direction_forward, speed, direction_forward, speed / 2);
  } else {
    AppMotor.control(direction_forward, speed / 2, direction_forward, speed);
  }
  delay(2000);
  AppMotor.stop();
}

void loop() {
  if (Serial.available() > 0) {  // Checks whether data is comming from the serial port
    c = Serial.read();
    
    switch (c) {
      case 'l': turn(true, speed); break;
      case 'r': turn(false, speed); break;
    }
    /*
    Serial.print("got ");
    Serial.println(c);
    */
  } 
  //Serial.println("ready 2");
}
