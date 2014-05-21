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
This class is used for interfacing the AK8975 magnometer found in the MPU 9150
v 1.0
by Ned Danyliw
*/

#include <Wire.h>

//Magnetometer Registers
#define AK8975_RA_MAG_ADDRESS_DEFAULT 0x0C //address in MPU9150
#define AK8975_RA_MAG_ADDRESS AK8975_RA_MAG_ADDRESS_DEFAULT
#define AK8975_RA_MAG_XOUT_L   0x03
#define AK8975_RA_MAG_XOUT_H   0x04
#define AK8975_RA_MAG_YOUT_L   0x05
#define AK8975_RA_MAG_YOUT_H   0x06
#define AK8975_RA_MAG_ZOUT_L   0x07
#define AK8975_RA_MAG_ZOUT_H   0x08
#define AK8975_RA_CTRL 0x0A

//operating modes
#define AK8975_MODE_SLEEP 0x00
#define AK8975_MODE_SINGLE 0x01
#define AK8975_MODE_SELF_TEST 0x08
#define AK8975_MODE_FUSE 0x0F

class AK8975
{
public:
  AK8975();
  AK8975(int address);

  void init();

  void set_mode(int mode);
  void get_heading_x(int *mx);
  void get_heading_y(int *my);
  void get_heading_z(int *mz);

  void get_heading(int *mx, int *my, int *mz);

private:
  uint8_t dev_addr;
  /* data */
};