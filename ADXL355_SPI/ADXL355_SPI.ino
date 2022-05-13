/*
 ADXL355Z Accelerometer Sensor Display
 Shows the output of a ADXL355 accelerometer.
 Uses the SPI library. For details on the sensor, see:
 https://wiki.analog.com/resources/eval/user-guides/eval-adicup360/hardware/adxl355
 Created 13 May 2022
 by Teore
 */

#include <SPI.h>                       // SPI library - used for SPI communication

// ------------------------- Declare Constants ------------------------- //
// Memory register addresses:
#define STATUS        0x04              // define STATUS as 0x04
#define TEMP2         0x06              // define TEMP2  as 0x06
#define TEMP1         0x07              // define TEMP1  as 0x07
#define XDATA3        0x08              // define XDATA3 as 0x08
#define XDATA2        0x09              // define XDATA2 as 0x09
#define XDATA1        0x0A              // define XDATA1 as 0x0A
#define YDATA3        0x0B              // define YDATA3 as 0x0B
#define YDATA2        0x0C              // define YDATA2 as 0x0C
#define YDATA1        0x0D              // define YDATA1 as 0x0D
#define ZDATA3        0x0E              // define ZDATA3 as 0x0E
#define ZDATA2        0x0F              // define ZDATA2 as 0x0F
#define ZDATA1        0x10              // define ZDATA1 as 0x10
#define ACT_EN        0x24              // define ACT_EN as 0x24
#define RANGE         0x2C              // define RANGE as 0x2C
#define POWER_CTL     0x2D              // define POWER_CTL as 0x2D, power_register address
#define Reset         0x2F              // define RESET  as 0x2F

// Device values
#define RANGE_2G     0x01
#define RANGE_4G     0x02
#define RANGE_8G     0x03
#define MEASURE_MODE 0x00 // check all data 0x00 || only accelerometer 0x06

// Operations
#define READ_BYTE    0x01
#define WRITE_BYTE   0x00

// Pins used for the connection with the sensor
#define CHIP_SELECT_PIN 7

// Set scale by Range
float scale;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  SPI.begin();
  
  SPISettings setA(1000000, MSBFIRST, SPI_MODE0);      // Set SPI setting
  SPI.beginTransaction(setA);

  // Initalize the  data ready and chip select pins:
  pinMode(CHIP_SELECT_PIN, OUTPUT);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Reset ADXL355 Start!");
  writeRegister(Reset, 0X52); // Reset ADXL355
  delay(100);
  Serial.println("Reset Finish");

  Serial.print("POWER_CTL : ");
  Serial.println(MEASURE_MODE);
  writeRegister(POWER_CTL, MEASURE_MODE); // Enable measure mode
  delay(100);
  Serial.println(readRegistry(POWER_CTL));

  //Configure ADXL355:
  Serial.print("Range : ");
  Serial.println(RANGE_2G);
  writeRegister(RANGE, RANGE_2G); // 2G
  delay(100);
  Serial.println(readRegistry(RANGE));

  switch (readRegistry(RANGE)){
      case 0x01:
        scale = 3.9 / 1000000;
        break;
      case 0x02:
        scale = 7.8 / 1000000;
        break;
      case 0x03:
        scale = 15.6 / 1000000;
        break;
      default:
        scale =1.0; 
    }

  // Give the sensor time to set up:
  delay(100);
}

void loop() {
  // Read Acceleratatoin Data
  AccelerationData();
  // Read Temperature Data
  ReadTemp();
  delay(100);
  
  // Built in LED Blink
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void AccelerationData() {
  Serial.print("Accelerometer ADXL355 (x,y,z): ");            // print "Accelerometer ADXL355 (x,y,z): " tot the serial monitor
  Serial.print(readOrientation(XDATA1,XDATA2,XDATA3)); Serial.print(",");   // print function readOrientation with data from XDATA3 on the serial monitor
  Serial.print(readOrientation(YDATA1,YDATA2,YDATA3)); Serial.print(",");   // print function readOrientation with data from YDATA3 on the serial monitor
  Serial.print(readOrientation(ZDATA1,ZDATA2,ZDATA3));                      // print function readOrientation with data from ZDATA3 on the serial monitor*/
  
  Serial.println();                     // print a (new) line on the serial monitor
  delay(200);                           // a delay of 200 milliseconds
}

void ReadTemp(){
  //Read Re
  uint32_t v1 = readRegistry(TEMP2);
  uint32_t v2 = readRegistry(TEMP1);

  float temp = (v1<<8) + v2;
  temp = ((1852 - c)/9.05)+25.7;

  Serial.print("temperature : ");
  Serial.println(temp);
  delay(200);                           // a delay of 200 milliseconds
}

float readOrientation(int8_t add1,int8_t add2,int8_t add3){
  uint32_t v1 = readRegistry(add1);
  uint32_t v2 = readRegistry(add2);
  uint32_t v3 = readRegistry(add3);
  uint32_t data = (v3<<16)|(v2<<8)|v1;
  data = data >> 4;

  float value = (float)data;                        // value = data
  
  value = data>0X7FFFF?value-0XFFFFE:value;         // Ternary operator - if data is greater than 0x7FFFF then value-0XFFFFE else write value
  return value*scale;                               // returns value times scale(Range_2G 3.9, Range_4G 7.8, Range_8G 15.6)
}

/* 
 * Read registry in specific device address
 */
uint32_t readRegistry(uint8_t thisRegister) {
  uint32_t result = 0;
  byte dataToSend = (thisRegister << 1) | READ_BYTE;
  digitalWrite(CHIP_SELECT_PIN, LOW);
  SPI.transfer(dataToSend);
  result = SPI.transfer(0x00);
  digitalWrite(CHIP_SELECT_PIN, HIGH);
  return result;
}

/* 
 * Write registry in specific device address
 */
void writeRegister(byte thisRegister, byte thisValue) {
  byte dataToSend = (thisRegister << 1) | WRITE_BYTE;
  digitalWrite(CHIP_SELECT_PIN, LOW);
  SPI.transfer(dataToSend);
  SPI.transfer(thisValue);
  digitalWrite(CHIP_SELECT_PIN, HIGH);
}
