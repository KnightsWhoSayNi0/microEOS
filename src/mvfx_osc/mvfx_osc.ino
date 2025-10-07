/********************************************************************************

    microVision FX OSC Controller - microEOS

 ********************************************************************************
    version history

    yyyy-mm-dd    Vxx     Who             Comment

    2025-07-16    1.0.1   John Godman     Write ADC interface and OSC routines.
    
    2025-07-18    1.0.2   John Godman     Add pins for Teensy 4.0 and write
                                          LED routines.

    2025-07-19    1.0.3   John Godman     Bugfixes.

 ********************************************************************************/

/********************************************************************************
    includes
 ********************************************************************************/
#include <OSCBoards.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <OSCMatch.h>
#include <OSCMessage.h>
#include <OSCTiming.h>

#include <Encoder.h>
#include <string.h>

#include "definitions.h"

#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
SLIPEncodedUSBSerial SLIPSerial(thisBoardsSerialUSB);
#else
#include <SLIPEncodedSerial.h>
SLIPEncodedSerial SLIPSerial(Serial);
#endif


/********************************************************************************
    macros and constants
 ********************************************************************************/

const String HANDSHAKE_QUERY = "ETCOSC?";
const String HANDSHAKE_REPLY = "OK";

// keyboard
// selectors
const int rowPins[3] = {A0, A1, A2};
// inputs
const int colPins[7] = {D0, D1, D2, D3, D4, D5, D6};

// faders
const int chipPins[3] = {D3, D4, D5};
const int chanPins[3] = {D0, D1, D2};
const int dataPins[8] = {D0, D1, D2, D3, D4, D5, D6, D7};

/********************************************************************************
    global vars
 ********************************************************************************/

bool keyState[8][7];

uint8_t faderState[3][8];

Encoder wheel(WHEEL_B, WHEEL_A);
int32_t wheelVal = 0;

/********************************************************************************
    local functions
 ********************************************************************************/

void parseOSCMessage(String& msg)
{
  // check to see if this is the handshake string
  if (msg.indexOf(EOS_HANDSHAKE_QUERY) != -1)
  {
    // handshake string found!
    SLIPSerial.beginPacket();
    SLIPSerial.write(
      (const uint8_t*)EOS_HANDSHAKE_REPLY.c_str(), 
      (size_t)EOS_HANDSHAKE_REPLY.length()
    );
    SLIPSerial.endPacket();

    delay(1);
    
    // fader setup with eos
    String masterFaderConf("/eos/fader/0/config/2");
    String gmFaderConf("/eos/fader/99/config/1");
    String subBankConf("/eos/fader/1/config/6");

    sendOSCMessage(masterFaderConf); // master pair
    sendOSCMessage(gmFaderConf); // grandmaster fader
    sendOSCMessage(subBankConf); // sub banks
  }
}

void sendSLIP(OSCMessage &msg)
{
  SLIPSerial.beginPacket();
  msg.send(SLIPSerial);
  SLIPSerial.endPacket();
}

void sendOSCMessage(String &address)
{
  OSCMessage msg(address.c_str());
  sendSLIP(msg);
}

void sendOSCMessage_int(String &address, int32_t value)
{
  OSCMessage msg(address.c_str());
  msg.add(value);
  sendSLIP(msg);
}

void sendOSCMessage_float(String &address, float value)
{
  OSCMessage msg(address.c_str());
  msg.add(value);
  sendSLIP(msg);
}

void writeLED(int LED, int status)
{
  pinMode(D0, OUTPUT);
  digitalWrite(D0, status);

  for (int bit = 0; bit < 3; bit++)
  {
    digitalWrite(rowPins[bit], (LED >> bit) & 1);
  }

  digitalWrite(LEDWR, LOW);
  delayMicroseconds(50);
  digitalWrite(LEDWR, HIGH);

  pinMode(D0, INPUT);
}

/********************************************************************************
    arduino setup
 ********************************************************************************/

void setup() 
{
  // initialize matricies

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      keyState[i][j] = false;
    }
  }

  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      faderState[i][j] = 0;
    }
  }


  for (int i = 0; i < 3; i++) 
  {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], LOW);
  }

  for (int i = 0; i < 7; i++) 
  {
    pinMode(colPins[i], INPUT_PULLUP);
  }

  pinMode(WR, OUTPUT);
  pinMode(RD, OUTPUT);
  pinMode(LEDWR, OUTPUT);
  pinMode(D7, OUTPUT);
  digitalWrite(WR, HIGH);
  digitalWrite(RD, HIGH);
  digitalWrite(LEDWR, HIGH);
  digitalWrite(D7, HIGH);

  // LED setup
  // cool little flash sequence

  pinMode(D0, OUTPUT);
  digitalWrite(LEDWR, LOW);
  
  digitalWrite(D0, HIGH);
  for (int i = 0; i < 8; i++)
  {
    for (int bit = 0; bit < 3; bit++)
    {
      digitalWrite(rowPins[bit], (i >> bit) & 1);
    }

    delay(50);
  }

  delay(250);

  digitalWrite(D0, LOW);
  for (int i = 0; i < 8; i++)
  {
    for (int bit = 0; bit < 3; bit++)
    {
      digitalWrite(rowPins[bit], (i >> bit) & 1);
    }

    delayMicroseconds(50);
  }

  delay(250);

  digitalWrite(D0, HIGH);
  for (int i = 0; i < 8; i++)
  {
    for (int bit = 0; bit < 3; bit++)
    {
      digitalWrite(rowPins[bit], (i >> bit) & 1);
    }

    delayMicroseconds(50);
  }

  digitalWrite(LEDWR, HIGH);
  pinMode(D0, INPUT);


  //Serial.begin(9600);

  // eos communication setup
  SLIPSerial.begin(115200);

  // this is necessary for reconnecting a device because it needs some time
  // for the serial port to open, but meanwhile the handshake message was
  // sent from Eos
  SLIPSerial.beginPacket();
  SLIPSerial.write(
    (const uint8_t*)EOS_HANDSHAKE_REPLY.c_str(), 
    (size_t)EOS_HANDSHAKE_REPLY.length()
  );
  SLIPSerial.endPacket();

  delay(500); // wait to start program loop
} 

/********************************************************************************
    main program loop
 ********************************************************************************/

void loop() 
{
  static String curMsg;
  int size;

  // handle OSC
  // Check to see if any OSC commands have come from Eos that we need to respond to.
  size = SLIPSerial.available();
  if (size > 0)
  {
      // Fill the msg with all of the available bytes
      while (size--)
          curMsg += (char)(SLIPSerial.read());
  }
  if (SLIPSerial.endofPacket())
  {
      parseOSCMessage(curMsg);
      curMsg = String();
  }

  // read keyboard

  // change D pinmode to input
  for (int i = 0; i < 7; i++) 
  {
    pinMode(colPins[i], INPUT_PULLUP);
  }

  for (int row = 0; row < 8; row++) 
  {
    // set binary row address on A0-A2
    for (int bit = 0; bit < 3; bit++) 
    {
      digitalWrite(rowPins[bit], (row >> bit) & 1);
    }

    digitalWrite(WR, LOW);

    delayMicroseconds(100);

    for (int col = 0; col < 7; col++) 
    {

      if (row == 0 && col == 6)
      {           // i believe this is a problem with my hardware
        continue; // temporary hack ; remove? TODO
      }

      int val = digitalRead(colPins[col]);
      
      if (val == HIGH && keyState[row][col] == false)
      {
        String keyMsg(eosKybdPre);
        
        if (keyState[1][0] == false) // is fn key down
        {
          keyMsg.concat(eosKybdLUT[row][col]);
        } else
        {
          if (keyState[7][5] == false) // is shift key down
          {
            keyMsg.concat(eosKybdFnLUT[row][col]);
          } else {
            keyMsg.concat(eosKybdFnShiftLUT[row][col]);
          }
        }
        
        sendOSCMessage(keyMsg);
      }

      keyState[row][col] = (val == HIGH);
    }

    digitalWrite(WR, HIGH);
  }

  // read faders

  // change D pinmode to output
  for (int i = 0; i < 8; i++) 
  {
    pinMode(dataPins[i], OUTPUT);

    if (i != 7)
    {
      digitalWrite(dataPins[i], LOW);
    } else
    {
      digitalWrite(dataPins[i], HIGH); // write D7 high
    }
  }

  for (int i = 0; i < 3; i++)
  {
    digitalWrite(chipPins[i], HIGH);
  }

  // beigin ADC

  for (int chip = 0; chip < 2; chip++) // ignore effects engine (chip 3) for now
  {
    for (int chan = 0; chan < 8; chan++)
    {
      digitalWrite(chipPins[chip], LOW); // select chip

      for (int bit = 0; bit < 3; bit++)
      {
        digitalWrite(chanPins[bit], (chan >> bit) & 1);
      }

      delayMicroseconds(50);

      digitalWrite(D7, LOW); // write pulse
      delayMicroseconds(1);
      digitalWrite(D7, HIGH);

      delayMicroseconds(500); // wait for conversion to finish

      digitalWrite(RD, LOW); // read mode, buffer

      // change D pinmode to input
      for (int i = 0; i < 8; i++) 
      {
        pinMode(dataPins[i], INPUT_PULLUP);
      }

      delayMicroseconds(15);

      uint8_t byteValue = 0;
      for (int j = 0; j < 8; j++)
      {
        byteValue |= (digitalRead(dataPins[j]) << j); // OR bits together to create byte
      }

      digitalWrite(RD, HIGH);

      if (faderState[chip][chan] != byteValue)
      {
        faderState[chip][chan] = byteValue;

        String faderMsg(eosFaderPre);
        faderMsg.concat(eosFaderLUT[chip][chan]);
        sendOSCMessage_float(faderMsg, (float)faderState[chip][chan] / 255.0f);
      }

      // restore pin states for new cycle
      for (int i = 0; i < 8; i++) 
      {
        pinMode(dataPins[i], OUTPUT);

        if (i != 7)
        {
          digitalWrite(dataPins[i], LOW);
        } else
        {
          digitalWrite(dataPins[i], HIGH); // write D7 high
        }
      }

      for (int i = 0; i < 3; i++)
      {
        digitalWrite(chipPins[i], HIGH);
      }
    }

    // restore pin modes
    for (int i = 0; i < 8; i++) 
    {
      pinMode(dataPins[i], OUTPUT);

      if (i != 7)
      {
        digitalWrite(dataPins[i], LOW);
      } else
      {
        digitalWrite(dataPins[i], HIGH); // write D7 high
      }
    }

    digitalWrite(chipPins[chip], HIGH);

    for (int i = 0; i < 3; i++)
    {
      digitalWrite(chipPins[i], HIGH);
    }
  }

  // wheel

  wheelVal = wheel.readAndReset();
  if (wheelVal != 0)
  {
    String msg(eosWheelPre);
    sendOSCMessage_int(msg, wheelVal);
  }

  delay(20);
}
