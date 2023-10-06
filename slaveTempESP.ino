#include <HardwareSerial.h>
#include <AceCRC.h>

HardwareSerial SerialPort(2); // use UART2

using namespace ace_crc::crc16ccitt_byte;

const int BUFFER_SIZE = 100;
char buf[BUFFER_SIZE];
String incomingStr;
String outgoingStr = "";

void setup()  
{
  SerialPort.begin(4800, SERIAL_8N1, 16, 17); 
  Serial.begin(4800);
}


void loop()  
{ 
  if (SerialPort.available() > 0)
  {
    SerialPort.readBytesUntil('\n', buf, BUFFER_SIZE);
    incomingStr = String(buf);
    // Serial.println(incomingStr);
  }
  
  if (incomingStr.substring(0, 2) == "02")    // check if is slave 2 or not
  {
    outgoingStr += "02";
    if (incomingStr.substring(2, 4) == "03")
    {
      outgoingStr += "03";
      if (incomingStr.substring(4, 8) == "0002")
      {
        outgoingStr += "01";
        outgoingStr += String(random(512));
        ADU_Response(outgoingStr, "");
        outgoingStr = "";
      }
      else
      {
        SerialPort.print("83");
        SerialPort.println("02             "); 
      }
    }
    else 
    {
      SerialPort.print("83");
      SerialPort.println("01               "); 
    }  
  }
  incomingStr = ""; 
  delay(100);
}

void ADU_Response(String data, String crc_in)
{
  SerialPort.print("Temp {");
  SerialPort.print(data);
  SerialPort.print(crc_in);
  SerialPort.println("}     ");
}








