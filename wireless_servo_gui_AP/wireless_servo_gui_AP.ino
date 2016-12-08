/*
  Wireless Servo Control, with ESP as Access Point

  Usage: 
    Connect phone or laptop to "ESP Whee" wireless network
    Go to 192.168.4.1. 
    A webpage with four buttons should appear. Click them to move the robot.

  Installation: 
    In Arduino, go to Tools > ESP8266 Sketch Data Upload to upload the files from ./data to the ESP
    Then, in Arduino, compile and upload sketch to the ESP

  Requirements:
    Arduino support for ESP8266 board
      In Arduino, add URL to Files > Preferences > Additional Board Managers URL.
      See https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon

    Websockets library
      To install, Sketch > Include Library > Manage Libraries... > Websockets > Install
      https://github.com/Links2004/arduinoWebSockets
    
    ESP8266FS tool
      To install, create "tools" folder in Arduino, download, and unzip. See 
      https://github.com/esp8266/Arduino/blob/master/doc/filesystem.md#uploading-files-to-file-system


  Note that one of the motor pins overlaps with LED pin D4, so when the left wheel is going CW, the LED light close the ESP chip will turn on
  Note that we opted to swap the motor wires on the right motor so that LOW is backwards for both motors
  
  We define arbitrarily Left CW = backwards
  
  Hardware: 
  * NodeMCU Amica DevKit Board (ESP8266 chip)
  * Motorshield for NodeMCU 
  * 2 servos
  * Paper chassis

  modified Nov 2016
  Nancy Ouyang
*/

#include <Arduino.h>

#include <Servo.h>

#include "debug.h"
#include "file.h"
#include "server.h"


const int LED_PIN = D0;
#define LED_ON digitalWrite(LED_PIN, LOW)
#define LED_OFF digitalWrite(LED_PIN, HIGH)

const int SERVO_LEFT = D1;
const int SERVO_RIGHT = D2;
Servo servo_left;
Servo servo_right;


// WiFi AP parameters
const char* ap_ssid = "ESP Whee2";
const char* ap_password = "my_password";

// WiFi STA parameters
const char* sta_ssid = 
  "...";
const char* sta_password = 
  "...";

void setup() {
    setupDebug();
    setupPins();

    setupSTA(sta_ssid, sta_password);
    setupAP(ap_ssid, ap_password);

    setupFile();
    registerPage("/", "text/html", readFile("/controls.html")
    registerPage("/style.css", "text/css", readFile("/style.css")

    setupHTML();
    setupWS(webSocketEvent);
    setupMDNS();
}

void loop() {
    wsLoop();
    httpLoop();
}


//
// Movement Functions //
//

int stop() {
  //digitalWrite(LED_PIN, HIGH);
  debug("stop");
  servo_left.write(90);
  servo_right.write(90);
}

int forward() {
  //digitalWrite(LED_PIN, LOW);
  debug("forward");
  servo_left.write(0);
  servo_right.write(180);
}

int backward() {
  debug("backward");
  servo_left.write(180);
  servo_right.write(0);
}

int left() {
  debug("left");
  servo_left.write(180);
  servo_right.write(180);
}

int right() {
  debug("right");
  servo_left.write(0);
  servo_right.write(0);
}



//
// Setup //
//

void setupPins() {
    // setup LEDs and Motors
    debug("Setup LED and motor pins");
    pinMode(LED_PIN, OUTPUT);    //Pin D0 is LED
    LED_ON;                      //Turn on LED

    servo_left.attach(SERVO_LEFT);
    servo_right.attach(SERVO_RIGHT);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: 
        {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
            break;
        }
        case WStype_TEXT:
            Serial.printf("[%u] get text: %s\n", num, payload);

            if (payload[0] == '#') {
                if(payload[1] == 'F') {
                  forward();
                }
                else if(payload[1] == 'B') {
                  backward();
                }
                else if(payload[1] == 'L') {
                  left();
                }
                else if(payload[1] == 'R') {
                  right();
                }
                else {
                  stop();
                }
            }

            break;
    }
}
