/*
The MIT License (MIT)

Copyright (c) 2014 Ned Danyliw

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
This program runs a single axis stabilization algorithm on the quadcopter.
It only stabilizes in pitch.
*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>  //include libm

#define UART_BAUD_RATE 57600
#include "mpu6050.h"
#include "mpu6050registers.h"
#include "i2cmaster.h"

#include <Wire.h>
#include "PID_v1.h"


#define MOTOR1 3
#define MOTOR2 5
#define MOTOR3 6
#define MOTOR4 9

#define XGYRO_OFFSET 16
#define YGYRO_OFFSET 16
#define ZGYRO_OFFSET 11


long *ptr = 0;
double qw = 1.0f;
double qx = 0.0f;
double qy = 0.0f;
double qz = 0.0f;
double roll = 0.0f;
double pitch = 0.0f;
double yaw = 0.0f;

double pitch_deg, roll_deg, yaw_deg;

int16_t mx = 0;
int16_t my = 0;
int16_t mz = 0;

int16_t gx, gy, gz;

double pitch_setpoint = 0.0f;
uint8_t thrust = 200;

int motor1, motor2, motor3, motor4;

double pitch_error;
double pitch_error_sum = 0.0f;

double pitch_rate;
double gyro_pitch_rate;

double kp_pitch_att = 4;
double ki_pitch_att = 0.05;
double kd_pitch_att = 0.0;

double kp_pitch_rate = .4;
double ki_pitch_rate = 0.005;
double kd_pitch_rate = 0.0;

double error;

long last = 0;
long time_elapsed;

//create PID controllers for pitch
PID pitch_pid_attitude(&pitch_deg, &pitch_error, &pitch_setpoint, kp_pitch_att, ki_pitch_att, kd_pitch_att, DIRECT);
PID pitch_pid_rate(&gyro_pitch_rate, &pitch_rate, &pitch_error, kp_pitch_rate, ki_pitch_rate, kd_pitch_rate, DIRECT);

void setup() {
    //set up 8 bit Fast PWM
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
    TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);

    //zero motors
    analogWrite(MOTOR1, 0);
    analogWrite(MOTOR3, 0);
    analogWrite(MOTOR2, 0);
    analogWrite(MOTOR4, 0);

    Wire.begin();
    Serial.begin(UART_BAUD_RATE);

    //init mpu6050
    mpu6050_init();
    delay(50);

    mpu6050_setXGyroOffset(XGYRO_OFFSET);
    mpu6050_setYGyroOffset(YGYRO_OFFSET);
    mpu6050_setZGyroOffset(ZGYRO_OFFSET);

    mpu6050_dmpInitialize();
    mpu6050_dmpEnable();
    delay(10);

    //get gyro reading
    mpu6050_getRawGyro(&gx, &gy, &gz);

    //initialize PID
    pitch_pid_attitude.SetSampleTime(50);
    pitch_pid_attitude.SetOutputLimits(-180, 180);

    pitch_pid_rate.SetSampleTime(50);
    pitch_pid_rate.SetOutputLimits(-255+thrust, 255-thrust);

    delay(5000);
    last = millis();
    pitch_deg = -pitch * 180.0/3.1416;
    gyro_pitch_rate = (gx - XGYRO_OFFSET)/MPU6050_GYRO_LSB_2000;
    pitch_pid_attitude.SetMode(AUTOMATIC);
    pitch_pid_rate.SetMode(AUTOMATIC);
}

void loop() {
    time_elapsed = millis();
    if(mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz)) {
        mpu6050_getRollPitchYaw(qw, qx, qy, qz, &roll, &pitch, &yaw);
    }
    // read_compass(&mx, &my, &mz);
    // uart_puts("hello world4!\n");

    mpu6050_getRawGyro(&gx, &gy, &gz);
    pitch_deg = -pitch * 180.0/3.1416;
    gyro_pitch_rate = (gx - XGYRO_OFFSET)/MPU6050_GYRO_LSB_2000;

    // error = pitch_setpoint - pitch;
    // pitch_error_sum += error * (millis() - last)/1000.0;
    // pitch_error = kp_pitch*error + ki_pitch * pitch_error_sum;
    // last = millis();

    pitch_pid_attitude.Compute();
    pitch_pid_rate.Compute();

    //assign values to motors
    motor1 = thrust + pitch_rate;
    motor2 = thrust + pitch_rate;
    motor3 = thrust - pitch_rate;
    motor4 = thrust - pitch_rate;

    //handle overflow
    if (motor1 < 0) {
        motor1 = 0;
    } else if (motor1 > 255) {
        motor1 = 255;
    }

    if (motor2 < 0) {
        motor2 = 0;
    } else if (motor2 > 255) {
        motor2 = 255;
    }

    if (motor3 < 0) {
        motor3 = 0;
    } else if (motor3 > 255) {
        motor3 = 255;
    }

    if (motor4 < 0) {
        motor4 = 0;
    } else if (motor4 > 255) {
        motor4 = 255;
    }

    // analogWrite(MOTOR1, 0);
    // analogWrite(MOTOR3, 0);
    // analogWrite(MOTOR2, 0);
    // analogWrite(MOTOR4, 0);

    // if (millis() - last > 500) {
    //     Serial.print("Pitch - ");
    //     Serial.println(pitch_deg);
    //     Serial.print("Pitch Rate - ");
    //     Serial.println(gyro_pitch_rate);
    //     Serial.print("Motor 1 - ");
    //     Serial.println(motor1);
    //     Serial.print("Motor 3 - ");
    //     Serial.println(motor3);
    //     Serial.println(millis() - time_elapsed);

    //     last = millis();
    // }

    //update motors
    analogWrite(MOTOR1, motor1);
    analogWrite(MOTOR3, motor3);
    analogWrite(MOTOR2, motor2);
    analogWrite(MOTOR4, motor4);

    // delay(3);
}
