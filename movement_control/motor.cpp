#include "motor.h"

/*Motor control*/
void Motor::init(void) {
  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_STBY, OUTPUT);
}

void Motor::control(boolean direction_A, uint8_t speed_A,
                    boolean direction_B, uint8_t speed_B) {
  digitalWrite(PIN_Motor_STBY, HIGH);

  digitalWrite(PIN_Motor_AIN_1, direction_A);
  analogWrite(PIN_Motor_PWMA, speed_A);

  digitalWrite(PIN_Motor_BIN_1, direction_B);
  analogWrite(PIN_Motor_PWMB, speed_B);
}

void Motor::stop(void) {
  analogWrite(PIN_Motor_PWMA, 0);
  analogWrite(PIN_Motor_PWMB, 0);
  digitalWrite(PIN_Motor_STBY, LOW);
}
