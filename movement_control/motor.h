#ifndef _motor_H_
#define _motor_H_

#include <arduino.h>

/*Motor*/
class Motor {
public:
  void init(void);
  void control(int16_t speed_A, int16_t speed_B);
  void stop(void);
private:
#define PIN_Motor_PWMA 5
#define PIN_Motor_PWMB 6
#define PIN_Motor_BIN_1 8
#define PIN_Motor_AIN_1 7
#define PIN_Motor_STBY 3

public:
#define speed_max 255
#define direction_forward HIGH
#define direction_backward LOW
#define direction_void 3
};

#endif
