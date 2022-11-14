const int displayCount = 4;
const int encodingsNumber = 16;

const int firstBlink = 400;
const int secoundBlink = 800;

const int minThreshold = 400;
const int maxThreshold = 600;

const int debounce = 80;
const int longPressTime = 1000;
const int minPressTime = 40;

const int dataPin = 12;   //DS
const int latchPin = 11;  // STCP
const int clockPin = 10;  //SHCP

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;
          
const int pinSw = 2;
const int pinY = A0;
const int pinX = A1;
          
int displayDigits[displayCount] = {
  segD1, segD2, segD3, segD4
};

int digits[displayCount];

int byteEncodings[encodingsNumber] = {
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};

byte swState = LOW,
     lastSwState = LOW,
     switchState = HIGH;

int xValue;
int yValue;

bool joyMovedX = false;
bool joyMovedY = false;

bool dpState = false;

bool selected = false;

int currentDisplay = 0;

unsigned long dpBlink;
unsigned long lastDebounceTime;
unsigned long pressedTime;
unsigned long releasedTime;
              
void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i < displayCount; ++i) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  pinMode(pinSw, INPUT_PULLUP);
  Serial.begin(9600);
}
void readXY() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
}

void loop() {
  readXY();
  incrementOrDecrementByMovingY();
  changeStateByPressDuration();
  writeNumber(currentDisplay, selected);
}

void incrementOrDecrementByMovingY() {
  if (yValue < minThreshold && !joyMovedY && !selected) {
    if (currentDisplay < 3)
      currentDisplay++;
    joyMovedY = true;
  }
  if (yValue > maxThreshold && !joyMovedY && !selected) {
    if (currentDisplay > 0)
      currentDisplay--;
    joyMovedY = true;
  }
  if (yValue >= minThreshold && yValue <= maxThreshold && !selected)
    joyMovedY = false;
}

void innitDisplays() {
  digits[0] = 0;
  digits[1] = 0;
  digits[2] = 0;
  digits[3] = 0;
  currentDisplay = 0;
}

void changeStateByPressDuration() {
  swState = digitalRead(pinSw);
  if (swState != lastSwState)
    lastDebounceTime = millis();

  if ((millis() - lastDebounceTime) > debounce) {
    if (swState != switchState) {
      switchState = swState;
      if (switchState == LOW)
        pressedTime = millis();
      else
        releasedTime = millis();
      long pressDuration = releasedTime - pressedTime;
      if (pressDuration > longPressTime && selected == 0)
        innitDisplays();
      else if (pressDuration > minPressTime)
        selected = !selected;
    }
  }
  if (selected) {
    if (xValue > maxThreshold && !joyMovedX) {
      if (digits[currentDisplay] > 0)
        digits[currentDisplay]--;
      joyMovedX = true;
    }
    if (xValue < minThreshold && !joyMovedX) {
      if (digits[currentDisplay] < 15)
        digits[currentDisplay]++;
      joyMovedX = true;
    }
    if (xValue >= minThreshold && xValue <= maxThreshold)
      joyMovedX = false;
  }
  lastSwState = swState;
}

// function for writing the digits on the display
void writeNumber(int currentDisplay, int selected) {
  for (int i = 0; i < displayCount; ++i) {
    int digitToWrite = 0;
    if (i == currentDisplay) { 
      if (selected) // if the digit display is selected, the dp must be on
        digitToWrite = byteEncodings[digits[i]] + 1; // incrementing the value with 1, corresponding with the dp led
      else { // if the digit display is not selected, the dp must blink
        digitToWrite = byteEncodings[digits[i]];
        if (millis() - dpBlink > firstBlink) {
          digitToWrite = byteEncodings[digits[i]] + 1;
        }
        if (millis() - dpBlink > secoundBlink) {
          digitToWrite = byteEncodings[digits[i]];
          dpBlink = millis();
        }
      }
    }
    else
      digitToWrite = byteEncodings[digits[i]];
    writeReg(digitToWrite);
    activateDisplay(i);
    delay(3);
  }
}

void activateDisplay(int displayNumber) {
  for (int i = 0; i < displayCount; ++i)
    digitalWrite(displayDigits[i], HIGH);
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeReg(int digit) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  digitalWrite(latchPin, HIGH);
}
