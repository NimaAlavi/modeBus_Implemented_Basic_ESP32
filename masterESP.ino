#include <HardwareSerial.h>
#include <AceCRC.h>

HardwareSerial SerialPort(2); // use UART2

using namespace ace_crc::crc16ccitt_byte;

const int BUFFER_SIZE = 100;
char buf_in[BUFFER_SIZE];
char buf_out[BUFFER_SIZE];

int delayTime = 1000;

// relay constant parameters
byte slaveRelayAddr = 0x01;
byte slaveTempAddr = 0x02;
byte writeCoilFunc = 0x05;
byte readCoilFunc = 0x01;
byte readRegFunc = 0x03;
byte startingAddrCoil1[2] = {0x00, 0x01};
byte startingAddrCoil2[2] = {0x00, 0x02};
byte startingAddrReg[2] = {0x00, 0x02};
byte valueCoil_on[2] = {0xFF, 0x00};
byte valueCoil_off[2] = {0x00, 0x00};
byte quantityofCoilsRelay[2] = {0x00, 0x02};
byte quantityofRegTemp[2] = {0x00, 0x02};

void setup()  
{
  SerialPort.begin(4800, SERIAL_8N1, 16, 17); 
  Serial.begin(4800);
}

void loop()  
{ 
  // Read the Tempreture
  ADU_Request(slaveRelayAddr, readCoilFunc, startingAddrCoil1, quantityofCoilsRelay);
  delay(5);
  // Read Tempreture Reg
  ADU_Request(slaveTempAddr, readRegFunc, startingAddrReg, quantityofRegTemp);

  delayTime = 0;
  while(delayTime <= 10000)
  {
    
    // request
    if (Serial.available() > 0)
    {
      Serial.readBytesUntil('\n', buf_out, BUFFER_SIZE);
      Serial.println(buf_out);
      
      // switch the relay situation
      if (String(buf_out) == "relay1=> on") 
      {
        ADU_Request(slaveRelayAddr, writeCoilFunc, startingAddrCoil1, valueCoil_on);
      }
      if (String(buf_out) == "relay1=>off") 
      {
        ADU_Request(slaveRelayAddr, writeCoilFunc, startingAddrCoil1, valueCoil_off);
      }
      if (String(buf_out) == "relay2=> on") 
      {
        ADU_Request(slaveRelayAddr, writeCoilFunc, startingAddrCoil2, valueCoil_on);
      }
      if (String(buf_out) == "relay2=>off") 
      {
        ADU_Request(slaveRelayAddr, writeCoilFunc, startingAddrCoil2, valueCoil_off);
      }
    }
    
    // response
    if (SerialPort.available() > 0)
    {
      SerialPort.readBytesUntil('\n', buf_in, BUFFER_SIZE);
      Serial.print("Response : ");
      Serial.println(buf_in);
    }
    delayTime += 100;
    delay(100);
  }

  
}


void ADU_Request(byte slaveAddr, byte functionCode, byte startingAddr[2], byte valueCoil[2])
{
  crc_t crc = crc_init();
  String str = "";

  str += String(slaveAddr);
  str += String(functionCode);
  str += String(startingAddr[0]);
  str += String(startingAddr[1]);
  str += String(valueCoil[0]);
  str += String(valueCoil[1]);

  int str_len = str.length() + 1;
  char char_array[str_len];
  str.toCharArray(char_array, str_len);

  crc = crc_calculate(char_array, str_len-1);
  
  SerialPort.printf("%.2X", slaveAddr);
  SerialPort.printf("%.2X", functionCode);
  SerialPort.printf("%.2X", startingAddr[0]);
  SerialPort.printf("%.2X", startingAddr[1]);
  SerialPort.printf("%.2X", valueCoil[0]);
  SerialPort.printf("%.2X", valueCoil[1]);
  SerialPort.println((unsigned long) crc, 16);
}









