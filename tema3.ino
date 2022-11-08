// declare all the segments pins
#include <Dictionary.h>
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int pinX = A0;
const int pinY = A1;
const int pinSW = 2;
const int segSize = 8;
int index = 0;
bool commonAnode = false;

int xValue = 0;
int yValue = 0;
int swState = LOW;
int lastSwState = LOW;

int segments[segSize] = {
	pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte savedStates[segSize] = {
  LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW
};
const int buffer = 4;

const int minThreshold = 400;
const int maxThreshold = 600;
bool joyMoved = false;

Dictionary &move = *(new Dictionary(23));

int state = 1;
int currentPin = pinDP;
byte currentPinState = LOW;
unsigned long lastTimeBlink = 0;
const int blinkTimeOn = 300;
const int blinkTimeOff = 100;

unsigned long lastDebounse = 0;
const int longTimeDebounse = 1000;
const int minReactionTime = 5;

int currentMove = 0;
int lastMove = 0;

byte state2SwRead = 1;
byte state2SwLastRead = 1;

const byte on = 0;
const byte off = 1;

void setup() {
  if (commonAnode == true) {
    state = !state;
  }

  for (int i = 0; i < segSize; i++){
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  Serial.begin(9600);
}

void loop() {
  if (state == 1)
    state1();
  
  if (state == 2)
    state2();
}

// covention 
// 0 - not move
// 1 - up
// 2 - down
// 3 - left
// 4 - right

int decide_movement(int x, int y, bool joyMoved)
{
  if (x > minThreshold && x < maxThreshold && y > minThreshold && y < maxThreshold)
  {
    joyMoved = false;
    return 0;
  }

  if (x > maxThreshold && joyMoved == false)
  {
    joyMoved = true;
    return 4;
  }

  if (x < minThreshold && joyMoved == false)
  {
    joyMoved = true;
    return 3;
  }

  if (y > maxThreshold && joyMoved == false)
  {
    joyMoved = true;
    return 2;
  }

  if (y < minThreshold && joyMoved == false)
  {
    joyMoved = true;
    return 1;
  }
}

void init_move()
{
  move("A2", "G");
  move("A3", "F");
  move("A4", "B");
  move("B1", "A");
  move("B2", "G");
  move("B3", "F");
  move("C1", "G");
  move("C2", "D");
  move("C3", "E");
  move("C4", "DP");
  move("D1", "G");
  move("D3", "E");
  move("D4", "C");
  move("E1", "G");
  move("E2", "D");
  move("E4", "C");
  move("B2", "G");
  move("B2", "G");
  move("D4", "C");
  move("E1", "G");
  move("E2", "D");
  move("E4", "C");
  move("F1", "A");
  move("F2", "G");
  move("F4", "B");
  move("G1", "A");
  move("G2", "D");
  move("DP3", "C");
}

int string_to_pin(String s)
{
  if(s == "A")
    return pinA;
  
  if(s == "B")
    return pinB;
  
  if(s == "C")
    return pinC;
  
  if(s == "D")
    return pinD;
  
  if(s == "E")
    return pinE;
  
  if(s == "F")
    return pinF;
  
  if(s == "G")
    return pinG;
  
  if(s == "DP")
    return pinDP;
}

String pin_to_string(int pin)
{
  if(pin == pinA)
    return "A";

  if(pin == pinB)
    return "B";
  
  if(pin == pinC)
    return "C";

  if(pin == pinD)
    return "D";
  
  if(pin == pinE)
    return "E";

  if(pin == pinF)
    return "F";
  
  if(pin == pinG)
    return "G";

  if(pin == pinDP)
    return "DP";
}

void init_display()
{
  for(int i = 0; i <= segSize; ++i)
  {
    digitalWrite(segments[i], savedStates[i]);
  }
  joyMoved = false;
}

void state1()
{
  init_display();

  if(millis() - lastTimeBlink > blinkTimeOn && currentPinState == HIGH)
  {
    lastTimeBlink = millis();
    currentPinState = !currentPinState;
  }

  if(millis() - lastTimeBlink > blinkTimeOff && currentPinState == LOW)
  {
    lastTimeBlink = millis();
    currentPinState = !currentPinState;
  }

  digitalWrite(currentPin, currentPinState);

  if(digitalRead(pinSW) == off)
  { 
    if(millis() - lastDebounse > longTimeDebounse)
    {
      for(int i = 0; i <= segSize; ++i)
      {
        savedStates[i] = LOW;
        digitalWrite(segments[i], LOW);
      }
      currentPin = pinDP;
    }
    else if(millis() - lastDebounse > minReactionTime)
      state = 2;
    lastDebounse = millis();
  }

  init_move();
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  currentMove = decide_movement(xValue, yValue, joyMoved);
  String key = pin_to_string(currentPin) + lastMove;
  if(move(key) && currentMove != lastMove)
  {
    digitalWrite(currentPin, savedStates[currentPin - buffer]);
    currentPin = string_to_pin(move[key]);
  }
  lastMove = currentMove;
}

void state2()
{
  init_display();

  yValue = analogRead(pinY);
  Serial.println(yValue);
  if (yValue > maxThreshold && joyMoved == false)
  {
    joyMoved = true;
    currentPinState = LOW;
    digitalWrite(currentPin, currentPinState);
    savedStates[currentPin - buffer] = currentPinState;
  }

  if (yValue < minThreshold && joyMoved == false)
  {
    joyMoved = true;
    currentPinState = HIGH;
    digitalWrite(currentPin, currentPinState);
    savedStates[currentPin - buffer] = currentPinState;
  }

  if (yValue > minThreshold && yValue < maxThreshold && joyMoved == true)
  {
    joyMoved == false;
  }

  state2SwLastRead = state2SwRead;
  state2SwRead = digitalRead(pinSW);

  if (state2SwLastRead == on && state2SwRead == off)
  {
    state = 1;
    lastDebounse = millis();
  }

}