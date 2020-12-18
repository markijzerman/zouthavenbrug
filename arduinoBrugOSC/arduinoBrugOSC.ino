// ZOUTHAVENBRUG OSC SENDER
// FROM ADXL345 ACCELEROMETER
// useful links
// https://howtomechatronics.com/tutorials/arduino/how-to-track-orientation-with-arduino-and-adxl345-accelerometer/

// LIBRARIES
#include <OSCMessage.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCMessage.h>
EthernetUDP Udp;
#include <Wire.h>  // Wire library - used for I2C communication

// SETTINGS
// the Arduino's IP
IPAddress ip(192, 168, 1, 2);
// Arduino's MAC address of the eth shield
byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0x2B, 0xE2};
// destination IP of Raspberry Pi
IPAddress outIp(192, 168, 1, 3);
// send port
const unsigned int outPort = 9999;
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float Z_out;  // Outputs. Now only set the Z.


void setup() {
  // setup stuff OSC
  Ethernet.begin(mac,ip);
    Udp.begin(8888);

  // setup stuff I2C
//  Serial.begin(9600); // for serial testing
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);
}

void loop(){
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
//  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
//  X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
//  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
//  Y_out = Y_out/256;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/256;
//  Serial.print("Xa= ");
//  Serial.print(X_out);
//  Serial.print("   Ya= ");
//  Serial.print(Y_out);
//  Serial.print("   Za= ");
//  Serial.println(Z_out);
  
  //the message wants an OSC address as first argument
  OSCMessage msg("/accel/Z");
  msg.add(Z_out);
  
  Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

  delay(10);
}
