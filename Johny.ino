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

#import "Motor.h"
#include <Ultrasonic.h>

Ultrasonic ultrasonic(12,13); //Ultrasonic ultrasonic(Trig,Echo);

Motor motorA = Motor(12, 3);
Motor motorB = Motor(13, 11);

String command = "NULL";// Command received (lowercase)
int flag = 0;// Makes sure we only run once per command

void setup() {
  // Init bluetooth serial
  Serial.begin(9600);
}

void loop() {
  // Check for collisions
  bool shouldAvoid = willCollide(5);

  // If some data is sent, read it and save it in the command variable
  if (shouldAvoid) {
    avoidObstacle();
  }
  
  if (Serial.available() > 0) {
    command = Serial.readString();
    command.toLowerCase();

    flag = 0;
  }

  // Parse the command
  if (flag == 0) {//Make sure we only execute this once per new command
    // Parse the command
    parseCommand();
    flag = 1;
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

bool willCollide(int distance) {
  int distanceToObstacle = ultrasonic.Ranging(CM);
  return (distanceToObstacle < distance); 
}

void avoidObstacle () {
   Serial.println("WARNING: Avoiding obstacle...");

   motorA.stop();
   motorB.stop();
   
   bool shouldAvoid = willCollide(5);
   while (shouldAvoid) {
     motorB.go(5);
   }
   
   Serial.println("SUCCESS: Obstacle avoided!");
}
