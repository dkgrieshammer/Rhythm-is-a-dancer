// These need to be included when using standard Ethernet
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "AppleMidi.h"

// ++++++++++++++++++++++++++++++++++++++++++++++
// WIFI Stuff
// ++++++++++++++++++++++++++++++++++++++++++++++
char ssid[] = "xxx"; //  your network SSID (name)
char pass[] = "xxx";    // your network password (use for WPA, or use as key for WEP)


// ++++++++++++++++++++++++++++++++++++++++++++++
// MIDI Stuff
// ++++++++++++++++++++++++++++++++++++++++++++++
unsigned long t0 = millis();
bool isConnected = false;
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI); // see definition in AppleMidi_Defs.h

// Calculate BPM Stuff
double bpm = 0;
double bps = 3; // Beat per Second
double interval = (1000/bps) / 24.0;  //interval is the number of milliseconds defined by tempo formula.
long prevMillis = 0;
long timePressed = 0;
long lastTimePressed = 0;
long timeDelta = 0;

// Button Stuff
int buttonPin = D5;
int buttonState = LOW;
int lastButtonState = LOW;
unsigned long lastButtonPress = 0;
bool pressedAlready = false;
int debounceTime = 50;


void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  networkInit();
}

void loop() {
  // Listen to incoming notes
  AppleMIDI.read(); //do we need that?
//  buttonRead();


  unsigned long currentMillis = millis();
  if (isConnected && (currentMillis - prevMillis) > interval) {
    // save the last time.
    prevMillis = currentMillis;
    AppleMIDI.sendClock();
  }

}


void buttonRead() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    // Reset Timer on change
    // so only if state is stable (didn't change for a while = this function is not called)
    // lastButtonPress and current millis start to differ
    lastButtonPress = millis();
  }
  
  // if button state was stable longer than debounceTime
  if ((millis() - lastButtonPress) > debounceTime) {

    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        //do nth
        pressedAlready = true;
      }

      if (buttonState == HIGH) {
        calculateBPM();
        pressedAlready = false;
      }

    }
  }
  lastButtonState = reading;
}

void calculateBPM () {
  timePressed = millis();
  timeDelta = timePressed - lastTimePressed;
//  Serial.print("time Delta");
//  Serial.println(timeDelta);
  bps = 1000.0 / timeDelta;
//  Serial.print("bps ");
//  Serial.println(bps);
  bpm = bps * 60.0;
  double tempo = 1000/bps;
  interval = tempo / 24;
//  Serial.print("bpm ");
//  Serial.println(bpm);
  lastTimePressed = timePressed;
}
