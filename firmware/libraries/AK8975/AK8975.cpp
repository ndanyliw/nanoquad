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

#include "AK8975.h"
#include <util/delay.h>

AK8975::AK8975() {
  dev_addr = AK8975_RA_MAG_ADDRESS;
}

AK8975::AK8975(int address) {
  dev_addr = (uint8_t)address;
}


void AK8975::init() {
  //set into single measurement mode
  set_mode(AK8975_MODE_SINGLE);
}


void AK8975::set_mode(int mode) {
  Wire.beginTransmission(dev_addr);
  Wire.write(AK8975_RA_CTRL);
  Wire.write(mode);
  Wire.endTransmission();
}


void AK8975::get_heading_x(int *mx) {
  Wire.beginTransmission(dev_addr);
  Wire.write(AK8975_RA_MAG_XOUT_L);
  Wire.endTransmission(false);
  Wire.requestFrom(dev_addr, (uint8_t)2);
  uint8_t buf[2];
  uint8_t index = 0;
  while(Wire.available()) {
    buf[index++] = Wire.read();
  }

  *mx = ((uint16_t)buf[1] << 8) | buf[0];
}

void AK8975::get_heading_y(int *my) {
  Wire.beginTransmission(dev_addr);
  Wire.write(AK8975_RA_MAG_YOUT_L);
  Wire.endTransmission(false);
  Wire.requestFrom(dev_addr, (uint8_t)2);
  uint8_t buf[2];
  uint8_t index = 0;
  while(Wire.available()) {
    buf[index++] = Wire.read();
  }

  *my = ((uint16_t)buf[1] << 8) | buf[0];
}

void AK8975::get_heading_z(int *mz) {
  Wire.beginTransmission(dev_addr);
  Wire.write(AK8975_RA_MAG_ZOUT_L);
  Wire.endTransmission(false);
  Wire.requestFrom(dev_addr, (uint8_t)2);
  uint8_t buf[2];
  uint8_t index = 0;
  while(Wire.available()) {
    buf[index++] = Wire.read();
  }

  *mz = ((uint16_t)buf[1] << 8) | buf[0];
}

void AK8975::get_heading(int *mx, int *my, int *mz) {
  // set_mode(AK8975_MODE_SINGLE);
  // _delay_ms(10);
  Wire.beginTransmission(dev_addr);
  Wire.write(AK8975_RA_MAG_XOUT_L);
  Wire.endTransmission(false);
  Wire.requestFrom(dev_addr, (uint8_t)6);
  uint8_t buf[6];
  uint8_t index = 0;
  while(Wire.available()) {
    buf[index++] = Wire.read();
  }

  *mx = ((uint16_t)buf[1] << 8) | buf[0];
  *my = ((uint16_t)buf[3] << 8) | buf[2];
  *mz = ((uint16_t)buf[5] << 8) | buf[4];
}