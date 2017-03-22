#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "Wiichuck.h"
#include <Wire.h>

void Wiichuck::init(int powerPin, int groundPin) {
	//// Set the output pins to VCC and GND
	//if(powerPin > 0) {
	//  pinMode(powerPin, OUTPUT);
	//  digitalWrite(powerPin, HIGH);
	//}
	//
	//if(groundPin > 0) {
	//  pinMode(groundPin, OUTPUT);
	//  digitalWrite(groundPin, LOW);
	//}
	
	delay(100);
	
	Wire.begin();
	Wire.beginTransmission(Address);
#if (ARDUINO >= 100)    
	Wire.write(0x40);
	Wire.write(0x00);
#else
	Wire.send(0x40);
	Wire.send(0x00);
#endif    
	Wire.endTransmission();
	
	// Set default calibration
	calib.joyX = calib.joyY = 128;
	calib.accelX = calib.accelY = calib.accelZ = 125; // accel and lsb together == 500.
	calib.lsbX = calib.lsbY = calib.lsbZ = 0;
}

uint8_t Wiichuck::poll() {
	Wire.requestFrom(Address, 6);// request data from nunchuck
      
	int bytes = 0;
	while(Wire.available() && bytes < 6) {
		// receive uint8_t as an integer
#if (ARDUINO >= 100)            
		data.buffer[bytes++] = decode(Wire.read());
#else
		data.buffer[bytes++] = decode(Wire.receive());
#endif        
	}

	if (bytes > 1)
	{
#define MYSERIAL SerialUSB

		MYSERIAL.print("Received ");
		MYSERIAL.print(bytes);
		MYSERIAL.println(" bytes");
		for (int i = 0; i < bytes; i++)
		{
			MYSERIAL.print(data.buffer[i]);
			MYSERIAL.print("   ");
		}

		MYSERIAL.print("\nData joyX: ");
		MYSERIAL.print(joyX());
		MYSERIAL.print("\tjoyY: ");
		MYSERIAL.print(joyY());
		MYSERIAL.print("\taccelX: ");
		MYSERIAL.print(accelX());
		MYSERIAL.print("\taccelY: ");
		MYSERIAL.print(accelY());
		MYSERIAL.print("\taccelZ: ");
		MYSERIAL.print(accelZ());
		MYSERIAL.print("\tbuttonZ: ");
		MYSERIAL.print(buttonZ());
		MYSERIAL.print("\tbuttonC: ");
		MYSERIAL.println(buttonC());
	
	}
      
	// send request for next data payload
	Wire.beginTransmission(Address);
#if (ARDUINO >= 100)        
	Wire.write(0x00); 
#else
	Wire.send(0x00); 
#endif
	Wire.endTransmission();
      
	delay(100);
	return bytes >= 5;
}

void Wiichuck::calibrate() {
	calib.joyX = data.parsed.joyX;
	calib.joyY = data.parsed.joyY;
	calib.accelX = data.parsed.accelX;
	calib.accelY = data.parsed.accelY;
	calib.accelZ = data.parsed.accelZ;
	calib.lsbX = data.parsed.lsbX;
	calib.lsbY = data.parsed.lsbY;
	calib.lsbZ = data.parsed.lsbZ;
}

