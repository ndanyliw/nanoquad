/*
test sketch for mpu6050

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

import processing.opengl.*;
import processing.serial.*;

float[] q = {0,0,0,0};
float roll = 0;
float pitch = 0;
float yaw = 0;
  
void setup() {
  size(400, 460, P3D);
  //println(Serial.list());
  new Serial(this, "/dev/tty.usbserial-A900ceSL",  57600); 
  noLoop();
//  PFont font = loadFont("ArialUnicodeMS-48.vlw"); 
//  textFont(font, 14);
}

void draw() {
  background(#000000);
  fill(#ffffff);
  
  int transX = 0;
  int transY = 0;
  
  //print data  
  text("Quaternion:\n  qw: " + q[0] + "\n  qx: " + q[1] + "\n  qy: " + q[2] + "\n  qz: " + q[3], 10, 20);

  //draw rotating cube
  transX = width/2;
  transY = height/2;
  translate(transX, transY);
  pushMatrix();
  draw_rotocube();
  popMatrix();
  translate(-transX, -transY);
  
  //draw pitch, roll and yaw
  transX = 55;
  transY = 420;
  translate(transX, transY);
  pushMatrix();
  draw_pitchrollyaw();
  popMatrix();
  translate(-transX, -transY);
}

/*
draw a rotating cube
*/
void draw_rotocube() {
  
  //ref:
  //  1 - [J. B. Kuipers] Quaternions and Rotation Sequences: A Primer with Applications to Orbits, Aerospace and Virtual Reality
  //  2 - [Sebastian O.H. Madgwick] An ecient orientation filter for inertial and inertial/magnetic sensor arrays
  //  3 - [Jay A. Farrell] Aided Navigation GPS with High Rate Sensors
  
  //pre notes: for this purpose i do not use math notation, sorry :)
  
  //reference frames:
  // * 0e,Xe,Ye,Ze  =  inertial frame - Earth Centered Inertial (ECI)
  // * 0s,Xs,Ys,Zs  =  sensor frame - the sensor board frame
  // * 0d,Xd,Yd,Zd  =  display frame - the display system reference frame ("processing")
  //notation: is O=center X,Y,Z=axis
  
  //sensor frame return values according to the intertial frame
  //i.e. accel/gyro/magn return values according to physic properties of the intertial frame (earth)
  //an alghotim process those values to return a quaternion that defines the sensor attitude respet to intertial frame
  //  Sq = [q0, q1, q2, q3]
  //sensor quaternion can be translated to aerospace sequence of yaw/pitch/roll angles
  //  yaw = atan2(2*q1*q2 - 2*q0*q3, 2*(q0^2) + 2*(q1^2) - 1)
  //  pitch = -asin(2*q1*q3 + 2*q0*q2)
  //  roll = atan2(2*q2*q3 - 2*q0*q1, 2*(q0^2) + 2*(q3^2) - 1)
  //using the aerospace sequence the sensor attitude can be obtain applying those rotations (in order)
  //1. rotate sensor by yaw angle around sensor Z axis
  //   yaw -> Zs
  //2. rotate sensor by pitch angle around sensor Y axis
  //   pitch -> Ys
  //3. rotate sensor by roll angle around sensor X axis
  //   roll -> Xs
  //defined as:
  //   Rse = yaw -> Zs  +  pitch -> Ys  +  roll -> Xs
    
  //to print a 3d object on a display, we should know the display frame
  //in case of "processing" axis are transformed to fit a better 3d view
  // * Ze = Yd  =  inertial frame Z is Y "processing" axis (rotateY)
  // * Ye = Xd  =  inertial frame Y is X "processing" axis (rotateX)
  // * Xe = Zd  =  inertial frame X is Z "processing" axis (rotateZ)
  
  //to get the Euler angles to go from the rotated sensor frame to the inertial frame just negate the angles and reverse the order
  //so, the rotation sequence
  //   Rse = yaw -> Zs  +  pitch -> Ys  +  roll -> Xs
  //can be converted to
  //   Res = -roll -> Xe  +  -pitch -> Ye  +  -yaw -> Ze
  
  //resulting rotation is:
  //   Rds = -roll -> Zd  +  -pitch -> Xd  +  -yaw -> Yd
  //"processing" code is:
  //1. rotateZ(-roll);
  //2. rotateX(-pitch);
  //3. rotateY(-yaw);
  
  rotateZ(-roll);
  rotateX(-pitch); 
  rotateY(-yaw);
      
  //axis
  strokeWeight(2);
  stroke(255, 0, 0);
  line(-100, 0, 0, 100, 0, 0);
  translate(+100, 0, 0);
  box(10,10,10);
  translate(-100, 0, 0);
  stroke(0, 255, 0);
  line(0, -100, 0, 0, 100, 0);
  translate(0, -100, 0);
  box(10,10,10);
  translate(0, +100, 0);
  stroke(0, 0, 255);
  line(0, 0, -100, 0, 0, 100);
  translate(0, 0, +100);
  box(10,10,10);
  translate(0, 0, -100);
  
  //box
  strokeWeight(2);
  stroke(#009999);
  fill(#009999, 220);
  box(100,100,100);
  
}

/*
draw pitch, roll and yaw indicators
*/
void draw_pitchrollyaw() {
  //roll circle
  pushMatrix();
  strokeWeight(2);
  stroke(#009999);
  fill(#009999, 220);
  ellipse(25, -25, 50, 50);
  
  fill(255);
  text("roll: " + nf(roll,1,5), 0, 20);
   
  translate(25, -25);
  strokeWeight(2);
  rotateZ(roll);
  stroke(255, 0, 0);
  line(-25, 0, 0, 25, 0, 0);
  popMatrix();
  
  translate(120, 0);
  //pitch circle
  pushMatrix();
  strokeWeight(2);
  stroke(#009999);
  fill(#009999, 220);
  ellipse(25, -25, 50, 50);

  fill(255);
  text("pitch: " + nf(pitch,1,5), 0, 20);
  
  translate(25, -25);
  strokeWeight(2);
  rotateZ(pitch);
  stroke(0, 255, 0);
  line(-25, 0, 0, 25, 0, 0);
  popMatrix();
  
  translate(120, 0);
  //yaw circle
  pushMatrix();
  strokeWeight(2);
  stroke(#009999);
  fill(#009999, 220);
  ellipse(25, -25, 50, 50);
  
  fill(255);
  text("yaw: " + nf(yaw,1,5), 0, 20);
  
  translate(25, -25);
  strokeWeight(2);
  rotateZ(yaw);
  stroke(0, 0, 255);
  line(-25, 0, 0, 25, 0, 0);
  popMatrix();
}
  
void serialEvent(Serial myPort) {  
  byte[] buf = new byte[128];
  int bufread = myPort.readBytesUntil((byte)'\n', buf);
  if(bufread == 29) {
    int i = 0;
    q[0] = Float.intBitsToFloat( (buf[(i+3)] & 0xff)<<24 | (buf[(i+2)] & 0xff)<<16 | (buf[(i+1)] & 0xff)<<8 | (buf[i] & 0xff) );
    i=i+4;
    q[1] = Float.intBitsToFloat( (buf[(i+3)] & 0xff)<<24 | (buf[(i+2)] & 0xff)<<16 | (buf[(i+1)] & 0xff)<<8 | (buf[i] & 0xff) );
    i=i+4;
    q[2] = Float.intBitsToFloat( (buf[(i+3)] & 0xff)<<24 | (buf[(i+2)] & 0xff)<<16 | (buf[(i+1)] & 0xff)<<8 | (buf[i] & 0xff) );
    i=i+4;
    q[3] = Float.intBitsToFloat( (buf[(i+3)] & 0xff)<<24 | (buf[(i+2)] & 0xff)<<16 | (buf[(i+1)] & 0xff)<<8 | (buf[i] & 0xff) );
    i=i+4;
    roll = Float.intBitsToFloat( (buf[(i+3)] & 0xff)<<24 | (buf[(i+2)] & 0xff)<<16 | (buf[(i+1)] & 0xff)<<8 | (buf[i] & 0xff) );
    i=i+4;
    pitch = Float.intBitsToFloat( (buf[(i+3)] & 0xff)<<24 | (buf[(i+2)] & 0xff)<<16 | (buf[(i+1)] & 0xff)<<8 | (buf[i] & 0xff) );
    i=i+4;
    yaw = Float.intBitsToFloat( (buf[(i+3)] & 0xff)<<24 | (buf[(i+2)] & 0xff)<<16 | (buf[(i+1)] & 0xff)<<8 | (buf[i] & 0xff) );
    i=i+4;
  }
  redraw();
}


