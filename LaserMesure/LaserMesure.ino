//#include <SPI.h>
#include <stdio.h>
#include "stdinout.h"
#include "blink.h"
#include "timeout.h"
#include "parser.h"
#include <SoftwareSerial.h>

#define UART_DEBUG

#define LASER_OPEN_CMD        (char)'O'
#define LASER_CLOSE_CMD       (char)'C'
#define LASER_MESURE_CMD      (char)'D'
#define LASER_FAST_MESURE_CMD (char)'F'
#define LASER_GET_INFO_CMD    (char)'S'
#define END_OF_PACKET         (char)'\n'

#define DEFAULT_BLINK_PERIOD  500u // ms
#define DEFAULT_TIMEOUT       10000u // ms

//unsigned int time = millis();
bool isInputData = false;

cBlink ledBlink(DEFAULT_BLINK_PERIOD);
cTimeout timeOut;
cParser parser;

SoftwareSerial mySerial(8, 9); // RX, TX

void setup()
{
  Serial.begin(19200);

#ifdef UART_DEBUG
  mySerial.begin(19200);
  parser.SetSerial(&mySerial);
//  mySerial.println("Hello, world?");
#endif
}

void LaserInit()
{
  Serial.write(LASER_OPEN_CMD);
  delay(10);
  Serial.write(LASER_MESURE_CMD);
  timeOut.TimeoutStart(DEFAULT_TIMEOUT);
  ledBlink.LedOn();
}

void serialEvent()
{
  char tmp;

  while(Serial.available())
  {
#ifdef UART_DEBUG
    tmp = Serial.read();
   // mySerial.write(tmp);
#endif
    parser.addNextChar(tmp);
    if(tmp == END_OF_PACKET)
    {
      isInputData = true;
      
      ledBlink.LedOn();
    }
  }
  
  timeOut.TimeoutStart(DEFAULT_BLINK_PERIOD);
}

void loop()
{
  //ledBlink.run();
  timeOut.run();

  if(timeOut.isTimeOver())
  {
    ledBlink.LedOff();
    isInputData = false;
    Serial.write(LASER_MESURE_CMD);
    timeOut.TimeoutStart(DEFAULT_TIMEOUT);
  }

  if(isInputData)
  {
    delay(10);
    isInputData = false;
    Serial.write(LASER_MESURE_CMD);
    ledBlink.LedOff();
  }
}