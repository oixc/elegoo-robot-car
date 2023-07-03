// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

float roll, pitch, yaw;

float elapsedTime, currentTime, previousTime;

struct robot{
  // position 
  float x;
  float y;
  float z;
  // velocity
  float vx;
  float vy;
  float vz;
  // acceleration
  float ax;
  float ay;
  float az;
};

robot bot;

//#define OUTPUT_READABLE_ROLL_PITCH_YAW
#define OUTPUT_READABLE_ROBOT_POSITION
//#define OUTPUT_READABLE_ROBOT_ACCELERATION

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
// #define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO


#define LED_PIN 13
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(9600);

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

    delay(5000);

    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // ax = ax / 16384.0 * 980.665; // for a 1g range divide by 16384.0 // 1g = 9.80665 m/s2 // multiply by 980.665 to get cm/s2
    // ay = ay / 16384.0 * 980.665;
    // az = ((az / 16384.0) - 1.0) * 980.665;
    // gx = gx / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
    // gy = gy / 131.0;
    // gz = gz / 131.0;

    previousTime = currentTime;        // Previous time is stored before the actual time read
    currentTime = millis();            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds

    // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
    roll = roll + gx / 131.0 * elapsedTime;  // deg/s * s = deg
    pitch = pitch + gy / 131.0 * elapsedTime;
    yaw = yaw + gz / 131.0 * elapsedTime;

    // velocity is updates by average acelleration over elapsed time
    //if (abs(ax))

    /*
    bot.ax = ax / 16384.0 * 980.665;
    bot.ay = ay / 16384.0 * 980.665;;
    bot.az = ((az / 16384.0) - 1.0) * 980.665;

    if (abs(bot.ax * elapsedTime) > 0.01) { bot.vx += bot.ax * elapsedTime; }
    if (abs(bot.ay * elapsedTime) > 0.01) { bot.vy += bot.ay * elapsedTime; }
    if (abs(bot.az * elapsedTime) > 0.01) { bot.vz += bot.az * elapsedTime; }

    bot.x += bot.vx * elapsedTime;
    bot.y += bot.vy * elapsedTime;
    bot.z += bot.vz * elapsedTime;
    */

    float speed = 0.0;  // in cm/s
    float steering_angle = yaw * 3.141582 / 180.0;
    bot.x += cos(steering_angle) * speed * elapsedTime;
    bot.y += sin(steering_angle) * speed * elapsedTime;
   
    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        //Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
    #endif

    #ifdef OUTPUT_READABLE_ROLL_PITCH_YAW
        Serial.print(roll); Serial.print("\t");
        Serial.print(pitch); Serial.print("\t");
        Serial.println(yaw);
    #endif

    #ifdef OUTPUT_READABLE_ROBOT_POSITION
      Serial.print(bot.x); Serial.print("\t");
      Serial.print(bot.y); Serial.print("\t");
      Serial.println(bot.z);
    #endif

    #ifdef OUTPUT_READABLE_ROBOT_ACCELERATION
      Serial.print(bot.ax * elapsedTime); Serial.print("\t");
      Serial.print(bot.ay * elapsedTime); Serial.print("\t");
      Serial.println(bot.az * elapsedTime);
    #endif

    #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    #endif

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}

