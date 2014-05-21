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
This program is adapted from the provided example by Davide Gironi. It recieves
the angle data over an RF24 link from a reciever running the mpu6050_wireless_dmp
program. It is intended to be run with the avr_sample_mpu6050_01 processing
sketch.
*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>

  //include libm

#define UART_BAUD_RATE 57600

//libraries to commuicate using NRF24L01+
#include <SPI.h>
#include "RF24.h"
#include "RF24_config.h"
#include "nRF24L01.h"
#include "printf.h"

#define RF_CE_PIN 9
#define RF_CSN_PIN 10
#define RF_CHAN 100
#define RF_PAYLOAD 17

long *ptr = 0;
double qw = 1.0f;
double qx = 0.0f;
double qy = 0.0f;
double qz = 0.0f;
double roll = 0.0f;
double pitch = 0.0f;
double yaw = 0.0f;

RF24 radio(RF_CE_PIN, RF_CSN_PIN);

const uint64_t pipes[2] = { 0xA0A0A0A0A0LL, 0xDEADBEEF11LL };

byte rf_payload[RF_PAYLOAD];

void setup() {
	Serial.begin(UART_BAUD_RATE);
	printf_begin();

	//setup RF link
	radio.begin();
	radio.setDataRate(RF24_2MBPS);
	radio.setChannel(RF_CHAN);

	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);

	radio.startListening();

	radio.printDetails();

	uint8_t payloadSize = radio.getPayloadSize();

	Serial.print("Payload size: ");
	Serial.println(payloadSize);

	Serial.println("Reciever Started");
}

void loop() {

	if (radio.available()) {
		bool done = false;
		while (!done) {
			done = radio.read(rf_payload, RF_PAYLOAD);

			//quaterion
			if (rf_payload[RF_PAYLOAD - 1] == 0x00) {
				//quaternion
				//qw
				Serial.write(rf_payload[0]);
				Serial.write(rf_payload[1]);
				Serial.write(rf_payload[2]);
				Serial.write(rf_payload[3]);
				//qx
				Serial.write(rf_payload[4]);
				Serial.write(rf_payload[5]);
				Serial.write(rf_payload[6]);
				Serial.write(rf_payload[7]);
				//qy
				Serial.write(rf_payload[8]);
				Serial.write(rf_payload[9]);
				Serial.write(rf_payload[10]);
				Serial.write(rf_payload[11]);
				//qz
				Serial.write(rf_payload[12]);
				Serial.write(rf_payload[13]);
				Serial.write(rf_payload[14]);
				Serial.write(rf_payload[15]);
			} else if (rf_payload[RF_PAYLOAD - 1] == 0x01) {
				//roll pitch yaw
				//roll
				Serial.write(rf_payload[0]);
				Serial.write(rf_payload[1]);
				Serial.write(rf_payload[2]);
				Serial.write(rf_payload[3]);
				//pitch
				Serial.write(rf_payload[4]);
				Serial.write(rf_payload[5]);
				Serial.write(rf_payload[6]);
				Serial.write(rf_payload[7]);
				//yaw
				Serial.write(rf_payload[8]);
				Serial.write(rf_payload[9]);
				Serial.write(rf_payload[10]);
				Serial.write(rf_payload[11]);

				Serial.write('\n');
			} else {
				Serial.println("error in communication");
			}
		}
	}
	delay(1);
}
