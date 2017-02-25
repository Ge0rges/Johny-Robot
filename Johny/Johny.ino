// Johny
// Robot Vehicle firmware for the Arduino platform
//
// Copyright (c) 2013 by Georges Kanaan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/**
 * @file johny.ino
 * @brief Arduino robot vehicle firmware.
 * @author Georges Kanaan
 */

#include "Motor.h"
#include <Ultrasonic.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5); // RX, TX

Ultrasonic ultrasonic(8, 9); //Ultrasonic ultrasonic(Trig,Echo);

Motor motorA = Motor(12, 3);
Motor motorB = Motor(13, 11);

String command = "NULL";// Command received (lowercase)
bool readCommand = false;// Makes sure we only run once per command
bool avoidingObstacle = false;

void setup() {
  // Init bluetooth serial
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  mySerial.begin(9600);

  // Setup Ultrasonic sensor
  pinMode(10, OUTPUT); // VCC pin
  pinMode(6, OUTPUT); // GND ping
  digitalWrite(10, HIGH); // VCC +5V mode
  digitalWrite(6, LOW);  // GND mode
}

void loop() {
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());

  // Avoid obstacles!
  if (willCollide(5)) {
    avoidObstacle();
  }
  
  // If some data is sent, read it and save it in the command variable
  if (Serial.available() > 0) {
    command = Serial.readString();
    command.toLowerCase();

    readCommand = false;
  }

  // Parse the command
  if (!readCommand) {//Make sure we only execute this once per new command
    // Parse the command
    parseCommand();
    readCommand = true;
  }
}

void parseCommand () {
  Serial.println("WARNING: Parsing command...");

  // Parse the command (left or right motor) by checking to see if the last character is an motor asssignement
  if (command.substring(command.length() - 2).compareTo("w") == 0) {
    motorA.go(command.toInt());
    motorB.go(command.toInt());

    Serial.println("SUCCESS: Continuing forward..");

  } else if (command.substring(command.length() - 1).compareTo("l") == 0) {
    motorA.go(command.toInt());

    Serial.println("SUCCESS: Turned left.");

  } else if (command.substring(command.length() - 1).compareTo("r")  == 0) {
    motorB.go(command.toInt());

    Serial.println("SUCCESS: Turned right.");

  }  else if (command.compareTo("stop") == 0) {
    motorA.stop();
    motorB.stop();

    Serial.println("SUCCESS: Stopped.");

  } else {
    // Other kind of command for now just return an error
    Serial.println("ERROR: Invalid command.");
  }
}

bool willCollide(int distance) {// Distance in centimeters
  int distanceToObstacle = ultrasonic.Ranging(CM);
  Serial.print(distanceToObstacle);
  Serial.println(" cm" );
  return (distanceToObstacle < distance);
}

void avoidObstacle () {
  if (!avoidingObstacle) {
    avoidingObstacle = true;
    Serial.println("WARNING: Avoiding obstacle...");

    motorA.stop();
    motorB.stop();

    while (willCollide(5)) {
      motorB.go(255);
      delay(1000);
      motorB.stop();
      delay(1000);
    }

    motorA.stop();
    motorB.stop();

    avoidingObstacle = false;
    Serial.println("SUCCESS: Obstacle avoided!");
  }
}
