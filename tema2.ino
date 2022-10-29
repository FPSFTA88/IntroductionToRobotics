const int redPeoplePin = 6;
const int greenPeoplePin = 5;

const int redCarPin = 12;
const int greenCarPin = 11;
const int yellowCarPin = 10;

const int buttonPin = 4;
const int buzzerPin = 3;

byte ledOn = HIGH;
byte buttonState = LOW;
byte buttonOff = HIGH;
byte greenPeopleLedState = LOW;

int buttonPressTime = 0;
const int greenTime = 8000;
const int yellowTime = 3000;
const int blinkGreenTime = 4000;
const int blinkTime = 250;
int blinkCounter = 0;
int state4Time = 0;

const int buzzerTone = 400;
const int buzzerTime = 500;
int beepCounter = 0;

int stateTime = 0;
int state = 1;

void setup() {
pinMode(redPeoplePin, OUTPUT);
pinMode(greenPeoplePin, OUTPUT);

pinMode(redCarPin, OUTPUT);
pinMode(greenCarPin, OUTPUT);
pinMode(yellowCarPin, OUTPUT);

pinMode(buttonPin, INPUT_PULLUP);
pinMode(buzzerPin, OUTPUT);

Serial.begin(9600);
}

void loop() {

  if(state == 1) //State 1
  {
    digitalWrite(greenCarPin, ledOn);
    digitalWrite(redPeoplePin, ledOn);
  
    buttonState = digitalRead(buttonPin);
    //Serial.println(buttonState);

    if(buttonState != buttonOff)
    {
      buttonPressTime = millis();
      state = 2;
    }
  }
  // else if (millis() - buttonPressTime < greenTime)
  //   Serial.println(millis() - buttonPressTime);
  else if(millis() - buttonPressTime > greenTime && state == 2) //State 2
  {
    digitalWrite(yellowCarPin, ledOn);
    digitalWrite(greenCarPin, !ledOn);

    //Serial.println("2");

    stateTime = millis();
    state = 3;
  }
  // else if (millis() - stateTime < yellowTime)
  //   Serial.println(millis() - stateTime);
  else if(millis() - stateTime > yellowTime && state == 3) //Start Sate 3
  {
    digitalWrite(yellowCarPin, !ledOn);
    digitalWrite(redPeoplePin, !ledOn);
    digitalWrite(greenPeoplePin, ledOn);
    digitalWrite(redCarPin, ledOn);

    //Serial.println("3");

    stateTime = millis();
    state = 4;
  }
  else if(millis() - stateTime < greenTime && state == 4) //State 3
  {
    if(millis() - stateTime - greenTime > buzzerTime * beepCounter) //millis() - stateTime - greenTime = state 3 time
      beepCounter++;

    if (beepCounter % 2 == 0)
      tone(buzzerPin, buzzerTone);
    else
      noTone(buzzerPin);
  }
  else if(millis() - stateTime > greenTime && state == 4) //State 4
  {
    digitalWrite(greenPeoplePin, greenPeopleLedState);

    if(greenPeopleLedState)
      tone(buzzerPin, buzzerTone);
    else
      noTone(buzzerPin);

    //Serial.println("4");

    if(millis() - stateTime - greenTime > blinkCounter * blinkTime) //millis() - stateTime - greenTime = state 4 Time
    {
      blinkCounter++;
      greenPeopleLedState = !greenPeopleLedState;
    }

    if(millis() - stateTime > greenTime + blinkGreenTime)
    {
      state = 1;
      digitalWrite(greenPeoplePin, !ledOn);
      digitalWrite(redCarPin, !ledOn);
    }
  }
}
