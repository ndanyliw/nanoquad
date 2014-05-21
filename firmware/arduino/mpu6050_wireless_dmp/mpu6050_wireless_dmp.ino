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
the angle data over an RF24 link to a reciever running the mpu6050_wireless_reciever
program. It is intended to be run with the avr_sample_mpu6050_01 processing
sketch.
*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>  //include libm

#define UART_BAUD_RATE 57600
#include <mpu6050.h>
#include <mpu6050registers.h>
#include <i2cmaster.h>

#include <SPI.h>
//libraries to commuicate using NRF24L01+
#include "RF24.h"
#include "RF24_config.h"
#include "nRF24L01.h"
#include "printf.h"

#define RF_CE_PIN 10
#define RF_CSN_PIN 8
#define RF_CHAN 100
#define RF_PAYLOAD 17

#define RX_ADDR "drone"
#define TX_ADDR "comp1"

#define MOTOR1 3
#define MOTOR2 5
#define MOTOR3 6
#define MOTOR4 9

#define XGYRO_OFFSET -16
#define YGYRO_OFFSET -16
#define ZGYRO_OFFSET 11

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

bool success = false;

byte rf_payload[RF_PAYLOAD];

RF24 radio(RF_CE_PIN, RF_CSN_PIN);

const uint64_t pipes[2] = { 0xA0A0A0A0A0LL, 0xDEADBEEF11LL };

void setup() {
	//set up motor pins and turn off
	analogWrite(MOTOR1, 0);
	analogWrite(MOTOR2, 0);
	analogWrite(MOTOR3, 0);
	analogWrite(MOTOR4, 0);


	Serial.begin(UART_BAUD_RATE);
	printf_begin();

	//init mpu6050
	mpu6050_init();
	delay(50);

	mpu6050_setXGyroOffset(XGYRO_OFFSET);
	mpu6050_setYGyroOffset(YGYRO_OFFSET);
	mpu6050_setZGyroOffset(ZGYRO_OFFSET);

	mpu6050_dmpInitialize();
	mpu6050_dmpEnable();
	delay(10);

	//setup RF link
	radio.begin();
	radio.setDataRate(RF24_2MBPS);
	radio.setChannel(RF_CHAN);

	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);

	radio.stopListening();

	radio.printDetails();

	// radio.write("Hello World", 11);
}

// int16_t ax, ay, az, gx, gy, gz;
void loop() {
	if(mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz)) {
		mpu6050_getRollPitchYaw(qw, qx, qy, qz, &roll, &pitch, &yaw);
		// read_compass(&mx, &my, &mz);
		// uart_puts("hello world4!\n");
	}
	delay(5);

	// mpu6050_getRawData(&ax, &ay, &az, &gx, &gy, &gz);

	// Serial.print(ax); Serial.print("\t");
	// Serial.print(ay); Serial.print("\t");
	// Serial.print(az); Serial.print("\t");
	// Serial.print(gx); Serial.print("\t");
	// Serial.print(gy); Serial.print("\t");
	// Serial.print(gz); Serial.print("\n");


	// //quaternion
	// ptr = (long *)(&qw);
	// Serial.write(*ptr);
	// Serial.write(*ptr>>8);
	// Serial.write(*ptr>>16);
	// Serial.write(*ptr>>24);
	// ptr = (long *)(&qx);
	// Serial.write(*ptr);
	// Serial.write(*ptr>>8);
	// Serial.write(*ptr>>16);
	// Serial.write(*ptr>>24);
	// ptr = (long *)(&qy);
	// Serial.write(*ptr);
	// Serial.write(*ptr>>8);
	// Serial.write(*ptr>>16);
	// Serial.write(*ptr>>24);
	// ptr = (long *)(&qz);
	// Serial.write(*ptr);
	// Serial.write(*ptr>>8);
	// Serial.write(*ptr>>16);
	// Serial.write(*ptr>>24);
	
	//quaternion
	ptr = (long *)(&qw);
	rf_payload[0] = (*ptr);
	rf_payload[1] = (*ptr>>8);
	rf_payload[2] = (*ptr>>16);
	rf_payload[3] = (*ptr>>24);
	ptr = (long *)(&qx);
	rf_payload[4] = (*ptr);
	rf_payload[5] = (*ptr>>8);
	rf_payload[6] = (*ptr>>16);
	rf_payload[7] = (*ptr>>24);
	ptr = (long *)(&qy);
	rf_payload[8] = (*ptr);
	rf_payload[9] = (*ptr>>8);
	rf_payload[10] = (*ptr>>16);
	rf_payload[11] = (*ptr>>24);
	ptr = (long *)(&qz);
	rf_payload[12] = (*ptr);
	rf_payload[13] = (*ptr>>8);
	rf_payload[14] = (*ptr>>16);
	rf_payload[15] = (*ptr>>24);

	//indicate first data packet
	rf_payload[16] = 0x0;

	success = radio.write(rf_payload, RF_PAYLOAD);
	// Serial.print("Sending first packet ... ");
	// Serial.println((success)?"ok":"failed");

	// //roll pitch yaw
	// ptr = (long *)(&roll);
	// Serial.write(*ptr);
	// Serial.write(*ptr>>8);
	// Serial.write(*ptr>>16);
	// Serial.write(*ptr>>24);
	// ptr = (long *)(&pitch);
	// Serial.write(*ptr);
	// Serial.write(*ptr>>8);
	// Serial.write(*ptr>>16);
	// Serial.write(*ptr>>24);
	// ptr = (long *)(&yaw);
	// Serial.write(*ptr);
	// Serial.write(*ptr>>8);
	// Serial.write(*ptr>>16);
	// Serial.write(*ptr>>24);

	//roll pitch yaw
	ptr = (long *)(&roll);
	rf_payload[0] = (*ptr);
	rf_payload[1] = (*ptr>>8);
	rf_payload[2] = (*ptr>>16);
	rf_payload[3] = (*ptr>>24);
	ptr = (long *)(&pitch);
	rf_payload[4] = (*ptr);
	rf_payload[5] = (*ptr>>8);
	rf_payload[6] = (*ptr>>16);
	rf_payload[7] = (*ptr>>24);
	ptr = (long *)(&yaw);
	rf_payload[8] = (*ptr);
	rf_payload[9] = (*ptr>>8);
	rf_payload[10] = (*ptr>>16);
	rf_payload[11] = (*ptr>>24);

	//indicate second data packet
	rf_payload[16] = 0x1;

	success = radio.write(rf_payload, RF_PAYLOAD);
	// Serial.print("Sending second packet ... ");
	// Serial.println((success)?"ok":"failed");

}
