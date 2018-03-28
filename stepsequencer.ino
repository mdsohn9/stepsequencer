int buttonPin[5] = {8, 9, 10, 33, 34};
int ledPins[4] = {2, 3, 4, 5};
boolean lastButtonState[6] = {LOW, LOW, LOW, LOW, LOW, LOW};
boolean buttonState[6] = {LOW, LOW, LOW, LOW, LOW, LOW};
boolean stepState[3][4] = {
  {false, false, false, false},
  {false, false, false, false},
  {false, false, false, false}
};

int tempo = 0;

int currentStep = 0;
unsigned long lastStepTime = 0;

int midiNote[3] = {35, 57, 63};

int currentChannel = 0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i <= 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  for (int i = 0; i <= 5; i++) {
    pinMode(buttonPin[i], INPUT);
  }
}

void loop() {
  Serial.println(currentChannel);
  updateChannel();
  sequence();
  checkButtons();
  setLeds();
}


void sequence() {
  int mappedTempo;
  tempo = analogRead(A5);
  mappedTempo = map(tempo, 0, 1023, 10, 300);

  if (millis() > lastStepTime + mappedTempo) {   //if its time to go to the next step...
    //    digitalWrite(ledPins[currentStep], LOW);  //turn off the current led

    for (int i = 0; i < 3; i++) {

      if (stepState[i][currentStep] == HIGH) {
        usbMIDI.sendNoteOn(midiNote[i], 127, 1);
      } else {
        usbMIDI.sendNoteOff(midiNote[i], 0, 1);
      }
    }
    currentStep = currentStep + 1;         //increment to the next step
    if (currentStep > 3) {
      currentStep = 0;
    }
    //    digitalWrite(ledPins[currentStep], HIGH); //turn on the new led

    lastStepTime = millis();               //set lastStepTime to the current time
  } while (usbMIDI.read()) {
  }
}

void updateChannel() {
  for (int i = 4; i <= 5; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      //     up button code
      if (i == 4) {
        currentChannel++;
        if (currentChannel > 2) {
          currentChannel = 0;
        }
      }

      //     down button code
      if (i == 5) {
        currentChannel--;
        if (currentChannel < 0) {
          currentChannel = 2;
        }
      }
    }
  }
}

void checkButtons() {
  for (int i = 0; i <= 3; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      if (stepState[currentChannel][i] == false) {
        stepState[currentChannel][i] = true;
      } else if (stepState[currentChannel][i] == true) {
        stepState[currentChannel][i] = false;
      }
    }
  }
}

/*
  void setLeds() {
  for (int i = 0; i <= 3; i++) {
    if (stepState[i] == true) {
  //      digitalWrite(ledPins[i], HIGH);
    } else if (stepState[i] == false) {
  //      digitalWrite(ledPins[i], LOW);
    }
  }
*/

void setLeds() {
  for (int i = 0; i <= 3; i++) {
    if (currentStep == i) {
      analogWrite(ledPins[i], 255);
    }
    else if (stepState[currentChannel][i] == true) {
      analogWrite(ledPins[i], 50);
    }
    else {
      analogWrite(ledPins[i], 0);
    }
  }
}

