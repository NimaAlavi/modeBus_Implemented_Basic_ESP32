#include <HardwareSerial.h>
#include <AceCRC.h>

HardwareSerial SerialPort(2); // use UART2

using namespace ace_crc::crc16ccitt_byte;

const int BUFFER_SIZE = 100;
char buf[BUFFER_SIZE];
const int yellowLed = 26;
const int blueLed = 12;
String incomingStr;
String outgoingStr = "";

void setup()  
{
  SerialPort.begin(4800, SERIAL_8N1, 16, 17); 
  Serial.begin(4800);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);
}


void loop()  
{ 
  if (SerialPort.available() > 0)
  {
    SerialPort.readBytesUntil('\n', buf, BUFFER_SIZE);
    incomingStr = String(buf);
    // Serial.println(incomingStr);
  }
  
  if (incomingStr.substring(0, 2) == "01")    // check if is slave 1 or not
  {
    if (incomingStr.substring(2, 4) == "05")
    {
      if (incomingStr.substring(4, 8) == "0001")
      {
        if (incomingStr.substring(8, 12) == "FF00")
        {
          digitalWrite(blueLed, HIGH);
          delay(10);
        }
        if (incomingStr.substring(8, 12) == "0000")
        {
          digitalWrite(blueLed, LOW);
          delay(10);
        } 
        ADU_Response(incomingStr.substring(0, 12), incomingStr.substring(12, 16));
      }
      else if (incomingStr.substring(4, 8) == "0002")
      {
        if (incomingStr.substring(8, 12) == "FF00")
        {
          digitalWrite(yellowLed, HIGH);
          delay(10);
        }
        if (incomingStr.substring(8, 12) == "0000")
        {
          digitalWrite(yellowLed, LOW);
          delay(10);
        } 
        ADU_Response(incomingStr.substring(0, 12), incomingStr.substring(12, 16));
      }
      else 
      {
        SerialPort.print("85");
        SerialPort.println("02"); 
      } 
    }
    else if (incomingStr.substring(2, 4) == "01")
    {
      outgoingStr += "0101";
      if (incomingStr.substring(4, 8) == "0001")
      {
        if (incomingStr.substring(8, 12) == "0001")
        {
          if (digitalRead(blueLed) == HIGH)
          {
            outgoingStr += "0001";
          }
          else
          {
            outgoingStr += "0000";
          }
          delay(10);
        }
        if (incomingStr.substring(8, 12) == "0002")
        {
          if (digitalRead(blueLed) == HIGH)
          {
            if (digitalRead(yellowLed) == HIGH)
            {
              outgoingStr += "0003";
            }
            if (digitalRead(yellowLed) == LOW)
            {
              outgoingStr += "0001";
            }
          }
          else
          {
            if (digitalRead(yellowLed) == HIGH)
            {
              outgoingStr += "0002";
            }
            if (digitalRead(yellowLed) == LOW)
            {
              outgoingStr += "0000";
            }
          }
          delay(10);
        }
        ADU_Response(outgoingStr, ""); 
        outgoingStr = "";
      }
      else 
      {
        SerialPort.print("85");
        SerialPort.println("03           "); 
      } 
    }
    else 
    {
      SerialPort.print("85");
      SerialPort.println("01             "); 
    }  
  }
  incomingStr = ""; 
  delay(100);
}

void ADU_Response(String data, String crc_in)
{
  crc_t crc = crc_init();

  int str_len = data.length() + 1;
  int crc_len = crc_in.length();
  char char_array[str_len];
  data.toCharArray(char_array, str_len);
  
  crc = crc_calculate(char_array, str_len-1);

  Serial.println(crc_in);
  Serial.println(crc);
  if (1) //String(crc) == crc_in
  {
    SerialPort.print("Relays {");
    SerialPort.print(data);
    SerialPort.print(crc_in);
    SerialPort.println("}           ");
  }
  // else
  // {
  //   SerialPort.print(data.substring(2, 4));
  //   SerialPort.println("04");                   // Exception code
  // }
}








