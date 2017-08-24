#include <Servo.h>

// constants - pins
const int redPin = 2;
const int greenPin = 3;
const int bluePin = 4;
const int buttonPin = 6;

// constants - declaring different modes for switch statement
const int swapMode = 1;
const int workMode = 2;
const int breakMode = 3;
const int standbyMode = 4;

// constants - timers
const long workTimer = 4000; // 10 seconds for test
const long swapTimer = 4000; // 2.5 second for test
const long shortBreakTimer = 4000; // 5 seconds for test
const long longBreakTimer = 4000; // 10 seconds for test
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// variables
int mode; // stores the current mode
int counter; // counter for checking when it is time for a long break
int pos = 0; // position of servo
int maxPos = 170; // max position of servo
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
bool open = false;

Servo servo; // servo variable to attach the servo to a pin

// timestorage
unsigned long previousMillis = 0; // will store last time LED was updated
unsigned long currentMillis; // will store current time

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  
  pinMode(buttonPin, INPUT);

  servo.attach(5);

  // start in standby mode
  mode = standbyMode;
  counter = 0;
}

void loop() {
  currentMillis = millis();

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has changed
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the mode if the new button state is HIGH
      if (buttonState == HIGH) {
        if(mode == standbyMode){
          mode = swapMode;
          previousMillis = currentMillis;
        }else{
          mode = standbyMode;
        }
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState
  lastButtonState = reading;
  
  if (buttonState == HIGH) {
    // reset when starting over
    if(mode == standbyMode){
      open = false;
      mode = swapMode;
      previousMillis = currentMillis;
      counter = 0;
    }else{
      mode = standbyMode;
    }
  }

  switch(mode){
    case standbyMode:
      standby();
      break;
    case swapMode:
      swap();
      break;
    case workMode:
      work();
      break;
    case breakMode:
      break_();
      break;
  }
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void standby(){
  setColor(150, 150, 150);
  
  // open when standing by and position of the servo is not already in the right spot
  if(pos != maxPos){
    for (pos = 0; pos <= maxPos; pos += 1) { // goes from 0 degrees to 170 degrees
      servo.write(pos); // tell servo to go to position in variable 'pos'
      delay(15);
    }
    pos = maxPos;
  }
}

void swap(){
  setColor(255, 0, 0); // red light
  if (currentMillis - previousMillis >= swapTimer) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    
    mode = workMode;

    // at the end of swap mode close
    for (pos = maxPos; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      servo.write(pos); // tell servo to go to position in variable 'pos'
      delay(15);
    }
    pos = 0;
  }
}

void work(){
  setColor(0, 213, 255); // 480 nm blue light
  if (currentMillis - previousMillis >= workTimer) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    
    counter++;
    mode = breakMode;


    // at the end of work mode open
    for (pos = 0; pos <= maxPos; pos += 1) { // goes from 0 degrees to 180 degrees
      servo.write(pos); // tell servo to go to position in variable 'pos'
      delay(15);
    }
    pos = 180;
  }
}

void break_(){ // "break_" because "break" is a reserved keyword for switch statements
  setColor(0, 255, 0); // green light
  
  if(counter == 4){
    if (currentMillis - previousMillis >= longBreakTimer) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      mode = swapMode;
      counter = 0;
    }
  }else{
    if (currentMillis - previousMillis >= shortBreakTimer) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      mode = swapMode;
    }
  }
}



