/*
 * Wiichuck library -- Talk to a Wii Nunchuck
 * Based off code from:
 * 2007 Tod E. Kurt, http://todbot.com/blog/
 * The Wii Nunchuck reading code originally from Windmeadow Labs
 *   http://www.windmeadow.com/node/42
 */

#ifndef _Wiichuck_h_
#define _Wiichuck_h_

#include <inttypes.h>
#include <Wire.h>

typedef struct {
	uint8_t joyX;
  uint8_t joyY;
  uint8_t accelX;
  uint8_t accelY;
  uint8_t accelZ;
  uint8_t buttonZ : 1;
  uint8_t buttonC : 1;
  uint8_t lsbX : 2;
  uint8_t lsbY : 2;
  uint8_t lsbZ : 2;
} WiichuckData;

class Wiichuck {
  private:
    static const int Address = 0x52;
    
    // Wiichuck "encodes" data in some silly way.
    static inline uint8_t decode(uint8_t b) { return (b ^ 0x17) + 0x17; }
    
    // This is the retrieved data, it can be
    // accessed either as an array of uint8_ts,
    // or parsed out as a struct.
    union {
      uint8_t buffer[6];
      WiichuckData parsed;
    } data;
    
		WiichuckData calib;
		
  public:
    // Set powerPin and groundPin to 0 if you've plugged
    // them into VCC and GND
    void init(int powerPin = 0, int groundPin = 0);
		   
    // Requests data from the nunchuck
    uint8_t poll();
    
    // Sets the zero values to whatever is currently being read.
    void calibrate();
    
    inline int joyX() { return (int)data.parsed.joyX - calib.joyX; }    
    inline int joyY() { return (int)data.parsed.joyY - calib.joyY; }

    inline int accelX() { return (int)((data.parsed.accelX << 2) | data.parsed.lsbX) - ((calib.accelX << 2) | calib.lsbX); }
    inline int accelY() { return (int)((data.parsed.accelY << 2) | data.parsed.lsbY) - ((calib.accelY << 2) | calib.lsbY); }
    inline int accelZ() { return (int)((data.parsed.accelZ << 2) | data.parsed.lsbZ) - ((calib.accelZ << 2) | calib.lsbZ); }
    
    inline uint8_t buttonZ() { return !data.parsed.buttonZ; }
    inline uint8_t buttonC() { return !data.parsed.buttonC; }
};

#endif
