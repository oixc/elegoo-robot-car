#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 accelgyro;

float yaw;
float yaw_rate;
float elapsedTime, currentTime, previousTime;

#include "motor.h"
Motor AppMotor;

int speed = 100;
float delta_speed = 0.0;
float target_angular_velocity = 0.0;
float error_angular_velocity;

String msg;
bool go = false;

void imu_init(MPU6050 accelgyro) {
// join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // use the code below to change accel/gyro offset values
  Serial.println("Updating internal sensor offsets...");
  // 884,   -3459,    1446,      68,      87,      95
  Serial.print(accelgyro.getXAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getYAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getZAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getXGyroOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getYGyroOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getZGyroOffset());
  Serial.print("\t");
  Serial.print("\n");
  accelgyro.setXAccelOffset(884);
  accelgyro.setYAccelOffset(-3459);
  accelgyro.setZAccelOffset(1446);
  accelgyro.setXGyroOffset(68);
  accelgyro.setYGyroOffset(87);
  accelgyro.setZGyroOffset(95);
  Serial.print(accelgyro.getXAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getYAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getZAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getXGyroOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getYGyroOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getZGyroOffset());
  Serial.print("\t");
  Serial.print("\n");
  accelgyro.CalibrateAccel(6);
  accelgyro.CalibrateGyro(6);
  Serial.print(accelgyro.getXAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getYAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getZAccelOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getXGyroOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getYGyroOffset());
  Serial.print("\t");
  Serial.print(accelgyro.getZGyroOffset());
  Serial.print("\t");
  Serial.print("\n");
}

void setup() {
  Serial.begin(9600);
  AppMotor.init();
  imu_init(accelgyro);
  delay(1000);

  Serial.println("ready.. awaiting go");
}

void loop() {
  previousTime = currentTime;                         // Previous time is stored before the actual time read
  currentTime = millis();                             // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000;  // Divide by 1000 to get seconds

  yaw_rate = accelgyro.getRotationZ() / 131.0;
  yaw = yaw + yaw_rate * elapsedTime;

  /*
  Serial.print(yaw);
  Serial.print(",");
  Serial.println(yaw_rate);
  */

  if (go) {
    error_angular_velocity = target_angular_velocity - yaw_rate;

    // proportional correction
    delta_speed += 0.1 * error_angular_velocity;

    int speed_right = speed + delta_speed;
    int speed_left = speed - delta_speed;
    speed_right = constrain(speed_right, -200, 200);
    speed_left = constrain(speed_left, -200, 200);

    Serial.print(speed);
    Serial.print(",");
    Serial.print(target_angular_velocity);
    Serial.print(",");
    Serial.println(yaw_rate);
    //Serial.print(",");
    //Serial.print(error_angular_velocity);
    //Serial.print(",");
    //Serial.println(delta_speed);

    AppMotor.control(speed_right, speed_left);
  }

  if (Serial.available() > 0) {
    char c = Serial.read();

    switch (c) {
      case 'g': go = true; break;
      case 's':
        go = false;
        AppMotor.stop();
        break;
      case 'z': target_angular_velocity = 0.0; break;
      case 'l': target_angular_velocity = 90.0; break;
      case 'r': target_angular_velocity = -90.0; break;
      case 'u': speed += 20.0; break;
      case 'd': speed -= 20.0; break;
    }
  }
}
