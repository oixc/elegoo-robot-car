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
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); 
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t");
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t");
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t");
    Serial.print("\n");
    accelgyro.setXAccelOffset(884);
    accelgyro.setYAccelOffset(-3459);
    accelgyro.setZAccelOffset(1446);
    accelgyro.setXGyroOffset(68);
    accelgyro.setYGyroOffset(87);
    accelgyro.setZGyroOffset(95);
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t");
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t");
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t");
    Serial.print("\n");
    accelgyro.CalibrateAccel(6);
    accelgyro.CalibrateGyro(6);
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t");
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t");
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t");
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t");
    Serial.print("\n");
}

void setup() {
  Serial.begin(9600);
  AppMotor.init();
  imu_init(accelgyro);
  delay(1000);
}

void loop() {
    previousTime = currentTime;        // Previous time is stored before the actual time read
    currentTime = millis();            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds

    yaw_rate = accelgyro.getRotationZ() / 131.0;
    yaw = yaw + yaw_rate * elapsedTime;

    Serial.print(yaw); 
    Serial.print("\t");
    Serial.println(yaw_rate);

    AppMotor.control(direction_forward, speed / 2, direction_forward, speed);
  
  delay(2000);
  AppMotor.stop();

}

