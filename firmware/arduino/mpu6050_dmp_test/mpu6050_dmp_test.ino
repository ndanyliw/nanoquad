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
This program is adapted from the provided example by Davide Gironi. It sends
the angle data over serial. It is intended to be run with the avr_sample_mpu6050_01 processing
sketch.
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



long *ptr = 0;
double qw = 1.0f;
double qx = 0.0f;
double qy = 0.0f;
double qz = 0.0f;
double roll = 0.0f;
double pitch = 0.0f;
double yaw = 0.0f;

int16_t mx = 0;
int16_t my = 0;
int16_t mz = 0;

void setup() {
	Wire.begin();
	Serial.begin(UART_BAUD_RATE);

	//init mpu6050
	mpu6050_init();
	delay(50);

	mpu6050_dmpInitialize();
	mpu6050_dmpEnable();
	delay(10);
}

void loop() {
	if(mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz)) {
		mpu6050_getRollPitchYaw(qw, qx, qy, qz, &roll, &pitch, &yaw);
		// read_compass(&mx, &my, &mz);
		// uart_puts("hello world4!\n");
	}
	delay(10);


	//quaternion
	ptr = (long *)(&qw);
	Serial.write(*ptr);
	Serial.write(*ptr>>8);
	Serial.write(*ptr>>16);
	Serial.write(*ptr>>24);
	ptr = (long *)(&qx);
	Serial.write(*ptr);
	Serial.write(*ptr>>8);
	Serial.write(*ptr>>16);
	Serial.write(*ptr>>24);
	ptr = (long *)(&qy);
	Serial.write(*ptr);
	Serial.write(*ptr>>8);
	Serial.write(*ptr>>16);
	Serial.write(*ptr>>24);
	ptr = (long *)(&qz);
	Serial.write(*ptr);
	Serial.write(*ptr>>8);
	Serial.write(*ptr>>16);
	Serial.write(*ptr>>24);

	//roll pitch yaw
	ptr = (long *)(&roll);
	Serial.write(*ptr);
	Serial.write(*ptr>>8);
	Serial.write(*ptr>>16);
	Serial.write(*ptr>>24);
	ptr = (long *)(&pitch);
	Serial.write(*ptr);
	Serial.write(*ptr>>8);
	Serial.write(*ptr>>16);
	Serial.write(*ptr>>24);
	ptr = (long *)(&yaw);
	Serial.write(*ptr);
	Serial.write(*ptr>>8);
	Serial.write(*ptr>>16);
	Serial.write(*ptr>>24);

	Serial.write('\n');
}
