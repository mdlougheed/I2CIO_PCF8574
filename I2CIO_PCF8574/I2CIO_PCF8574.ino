// Uncomment line below to include debug code
// #define DEBUG

// I2CIO_PCF8574
// Version 0.1 (NOV-2020)
// by Mark D. Lougheed - Project Struix

// Sets up the Project Struix I2CIO board (or Arduino Uno/Lilypad) 
// to emulate a Phillips/NXP PCF8574 8-bit I2C bus digital I/O expander
// < https://hackaday.io/project/174984-i2c-io >

// This code is in the public domain.


#include <Wire.h>

#define I2CADDRESS 0x20   // PCF8574 addresses are 0x20 through 0x27




struct registerFile {
  uint8_t  dataio;  // data input/output & control
} regFile;


uint8_t prevData;   // previously written data for i/o control
                    // 1's = inputs
                    // 0's = outputs

enum RWOp {WASREAD, WASWRITE};
uint8_t lastOp;     // Store whether last operation was a read or write


// Set the I/O pin directions depending on the previous data state
void set8574PinDirections(){
  pinMode( 8, (prevData & 0x01) ? INPUT_PULLUP : OUTPUT);
  pinMode( 9, (prevData & 0x02) ? INPUT_PULLUP : OUTPUT);
  pinMode(10, (prevData & 0x04) ? INPUT_PULLUP : OUTPUT);
  pinMode(11, (prevData & 0x08) ? INPUT_PULLUP : OUTPUT);

  pinMode( 4, (prevData & 0x10) ? INPUT_PULLUP : OUTPUT);
  pinMode( 5, (prevData & 0x20) ? INPUT_PULLUP : OUTPUT);
  pinMode( 6, (prevData & 0x40) ? INPUT_PULLUP : OUTPUT);
  pinMode( 7, (prevData & 0x80) ? INPUT_PULLUP : OUTPUT);
}




void setup() {
  // 8574 Power-on reset state - all inputs
  lastOp=WASWRITE;
  prevData=regFile.dataio=0xff;
  set8574PinDirections();

  // Start I2C and register interrupt service routines
  Wire.begin(I2CADDRESS);       // join i2c bus with address
  Wire.onReceive(receiveEvent); // register event for I2C Write
  Wire.onRequest(requestEvent); // register event for I2c Read

#ifdef DEBUG 
  Serial.begin(19200);
  Serial.println("I2CIO_PCF8574");
#endif
}


// Interrupt driven code, so loop does nothing but stand by.
void loop() {
// delay(100);
}




// function that executes whenever data is received from I2C master  (I2C Write)
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
#ifdef DEBUG 
        Serial.println("Begin I2CWrite");
#endif

uint8_t *regBase = (uint8_t*)&regFile;    // Pointer to base of register file
uint8_t reg=0;                            // only one register in 8574


  if(Wire.available() > 0){                   // Test if this is something more than just a "ping"
    while(Wire.available()){
      uint8_t value = Wire.read();            // incoming byte(s) are the data values to store in the register file

      if(reg < sizeof(struct registerFile)){  // The data register index must be within the register file bounds

#ifdef DEBUG 
        Serial.print(reg);        // print the register
        Serial.print("-->");
        Serial.println(value);    // print the value
#endif

        regBase[reg]=value;       // Assign the data value in the register file

        if(lastOp==WASREAD){
          prevData=0x00;          // set all outputs
          set8574PinDirections();
        }

        (value & 0x01) ? digitalWrite( 8, HIGH)  : digitalWrite( 8, LOW);
        (value & 0x02) ? digitalWrite( 9, HIGH)  : digitalWrite( 9, LOW);
        (value & 0x04) ? digitalWrite(10, HIGH)  : digitalWrite(10, LOW);  
        (value & 0x08) ? digitalWrite(11, HIGH)  : digitalWrite(11, LOW);
        
        (value & 0x10) ? digitalWrite( 4, HIGH)  : digitalWrite( 4, LOW);
        (value & 0x20) ? digitalWrite( 5, HIGH)  : digitalWrite( 5, LOW);
        (value & 0x40) ? digitalWrite( 6, HIGH)  : digitalWrite( 6, LOW);
        (value & 0x80) ? digitalWrite( 7, HIGH)  : digitalWrite( 7, LOW);

        prevData=value;
        lastOp=WASWRITE;
        
        
      }
#ifdef DEBUG
      else
        Serial.println("Register out of bounds");
#endif
        
      ++reg;
    }
  }
#ifdef DEBUG   
  else
    Serial.println("ping!");
#endif

#ifdef DEBUG 
        Serial.println("End I2CWrite");
#endif
}




// function that executes whenever data is requested by master (I2C Read)
// this function is registered as an event, see setup()
void requestEvent()
{
  if(lastOp==WASWRITE)
    set8574PinDirections();

  regFile.dataio=0;
  if(prevData & 0x01) regFile.dataio |= (digitalRead( 8) << 0);
  if(prevData & 0x02) regFile.dataio |= (digitalRead( 9) << 1);
  if(prevData & 0x04) regFile.dataio |= (digitalRead(10) << 2);
  if(prevData & 0x08) regFile.dataio |= (digitalRead(11) << 3);
   
  if(prevData & 0x10) regFile.dataio |= (digitalRead( 4) << 4);
  if(prevData & 0x20) regFile.dataio |= (digitalRead( 5) << 5);
  if(prevData & 0x40) regFile.dataio |= (digitalRead( 6) << 6);
  if(prevData & 0x80) regFile.dataio |= (digitalRead( 7) << 7);

  lastOp=WASREAD;

  Wire.write(regFile.dataio);
}



