// Uncomment line below to include debug code
// #define DEBUG

// I2CIO_PCF8574_Test
// Version 0.1 (NOV-2020)
// by Mark D. Lougheed - Project Struix

// Exercises the Project Struix I2CIO board (or Arduino Uno/Lilypad) 
// to drive an emulated Phillips/NXP PCF8574 8-bit I2C bus digital I/O expander
// < https://hackaday.io/project/174984-i2c-io >

// This code is in the public domain.

#include <Wire.h>

// Delay in ms between I2C stop and I2C start - known hardware limitation for ATmega48A/PA/88A/PA/168A/PA/328/P operating in TWI slave mode 
// Rev K - TWI Data setup time can be too short
// When running the device as a TWI slave with a system clock above 2MHz, the data setup time for the first
// bit after ACK may in some cases be too short. This may cause a false start or stop condition on the TWI
// line.
// Problem Fix/Workaround
// Insert a delay between setting TWDR and TWCR.
// Actual value may vary depending on host processor

#define I2C_DELAY delay(75)
//      {unsigned long startMillis=millis(); for(unsigned long currentMillis = millis(); currentMillis - startMillis <= regFile.stepDelay; currentMillis = millis()); }



#define PCF8574_ADDR 0x20
byte error;


void setup()
{

  delay(250); // Give the system a little time to settle.
              // Avoids initial "barf" of serial port garbage characters after restart.

  Wire.begin(2,0);  // For ESP8266 (Struix MAJIIC board) this sets up pins
                    // used for SDA and SCL respetively.
                    // Arduino boards can use plain old Wire.begin()

  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2CIO PCF8574 Test");

  Wire.beginTransmission(PCF8574_ADDR);
  if(error = Wire.endTransmission()){
    Serial.print("Device not found at I2C Address 0x");
    Serial.println(PCF8574_ADDR,HEX);
    Serial.println("Stopping.");
    while(1) delay(100);
  }
I2C_DELAY;
  
  Wire.beginTransmission(PCF8574_ADDR);
  Wire.write(0b11111110);  //8 is an output
  error = Wire.endTransmission();

I2C_DELAY;

}

// Toggle the #8 data I/O pin ("blink")
void loop() {
byte data;
  // put your main code here, to run repeatedly:

  Wire.beginTransmission(PCF8574_ADDR);
  Wire.write(0b11111111); //8 is an output
  error = Wire.endTransmission();
I2C_DELAY;

  delay(1000);
  
  Wire.beginTransmission(PCF8574_ADDR);
  Wire.write(0b11111110); //8 is an output
  error = Wire.endTransmission();
I2C_DELAY;

  delay(1000);


}
