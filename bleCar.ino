/*
  Copyright (c) 2015 Intel Corporation. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-
  1301 USA
*/

#include <CurieBle.h>

enum Dir {
 dStop = 0,
 dForward,
 dBack,
 dLeft,
 dRight
};

#define DIR_STOP  0x00
#define DIR_FOR   0x01
#define DIR_BACK  0x02
#define DIR_LEFT  0x03
#define DIR_RIGHT 0x04

#define PIN_FOR   0x08
#define PIN_BACK  0x09
#define PIN_LEFT  0x0a
#define PIN_RIGHT 0x0b



const int ledPin = 13; // set ledPin to on-board LED
const int buttonPin = 4; // set buttonPin to digital pin 4

BLEPeripheral blePeripheral; // create peripheral instance
BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service


// create switch characteristic and allow remote device to read and write
BLECharCharacteristic dirCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLECharCharacteristic speedCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1215", BLERead | BLEWrite);
// create button characteristic and allow remote device to get notifications
BLECharCharacteristic sensorCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1216", BLERead | BLENotify); // allows remote device to get notifications

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // use the LED on pin 13 as an output
  digitalWrite(ledPin,HIGH);
  pinMode(buttonPin, INPUT); // use button pin 4 as an input
  for(int i=8;i<12;i++)
   pinMode(i,OUTPUT);
  // set the local name peripheral advertises
  blePeripheral.setLocalName("bleCar");
  // set the UUID for the service this peripheral advertises:
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(dirCharacteristic);
  blePeripheral.addAttribute(speedCharacteristic);
  blePeripheral.addAttribute(sensorCharacteristic);

  dirCharacteristic.setValue(0);
  speedCharacteristic.setValue(0);
  sensorCharacteristic.setValue(0);

  // advertise the service
  blePeripheral.begin();
  digitalWrite(ledPin,LOW);
  Serial.println("Bluetooth device active, waiting for connections...");
}

void setDirection(Dir d) {
  switch(d){
      case dStop:
        Serial.println("Stopping!");
        for(int i=8;i<12;i++)
         digitalWrite(i,LOW);
        break;
      case dForward:
        Serial.println("Going forward!");
        digitalWrite(PIN_BACK,LOW);
        digitalWrite(PIN_LEFT,LOW);
        digitalWrite(PIN_RIGHT,LOW);
        digitalWrite(PIN_FOR,HIGH);
        break;
      case dBack:
        Serial.println("Going backward!");
        digitalWrite(PIN_FOR,LOW);
        digitalWrite(PIN_LEFT,LOW);
        digitalWrite(PIN_RIGHT,LOW);
        digitalWrite(PIN_BACK,HIGH);
        break;
      case dLeft:
        Serial.println("Going left!");
        digitalWrite(PIN_FOR,LOW);
        digitalWrite(PIN_RIGHT,LOW);
        digitalWrite(PIN_BACK,LOW);
        digitalWrite(PIN_LEFT,HIGH);
        break;
      case dRight:
        Serial.println("Going right!");
        digitalWrite(PIN_FOR,LOW);
        digitalWrite(PIN_BACK,LOW);
        digitalWrite(PIN_LEFT,LOW);
        digitalWrite(PIN_RIGHT,HIGH);
        break;

  }
}

int btnVal = 0;
int cnt = 0;
int speedVal = 0;
void loop() {
  // poll peripheral
  blePeripheral.poll();
  if(cnt++ > 1000){
   btnVal++;
   cnt=0;
  }
  
  // read the current button pin state
  char buttonValue = btnVal;

  // has the value changed since the last read
  boolean buttonChanged = (sensorCharacteristic.value() != buttonValue);

  if (buttonChanged) {
    // button state changed, update characteristics
    //dirCharacteristic.setValue(buttonValue);
    sensorCharacteristic.setValue(buttonValue);
  }

  if(speedCharacteristic.written()) {
     int val = speedCharacteristic.value();
     Serial.println("setting speed to " + String(val));
  }
  
  if (dirCharacteristic.written()) {
    // update LED, either central has written to characteristic or button state has changed
    int val = dirCharacteristic.value();
    Serial.println("got value " + String(val));
    if(val)
     digitalWrite(ledPin, HIGH);
    else
     digitalWrite(ledPin, LOW);
    Dir d;
     switch(val){
      case dStop:
        d = dStop;
        break;
      case dForward:
        d = dForward;
        break;
      case dBack:
        d = dBack;
        break;
      case dLeft:
        d = dLeft;
        break;
      case dRight:
        d = dRight;
        break;

  }
    setDirection(d);
    /*if (dirCharacteristic.value()) {
      Serial.println("got value " + dirCharacteristic.value());
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("LED off");
      digitalWrite(ledPin, LOW);
    }*/
  }
}
