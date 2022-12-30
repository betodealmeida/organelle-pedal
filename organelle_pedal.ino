/* A MIDI pedal for the Critter & Guitari Organelle.
 * 
 * An expression pedal sends CC numbers 21-24, depending on
 * the switch pressed. A sustain pedal sends CC 25 to control
 * the Aux button.
 */

#include <Bounce.h>

const int midiChannel = 15;
const int expressionCC[] = {21, 22, 23, 24};
const int sustainCC = 25;
Bounce switches[] = {
    Bounce(5, 10),
    Bounce(6, 10),
    Bounce(7, 10),
    Bounce(8, 10),
};

const int sustainPin = 15;
Bounce sustainPedal = Bounce(sustainPin, 10);  // 10 ms debounce

int activeKnob = -1;
int i, pinNumber;
int previousExpressionValue = -1;

void setup() {
  pinMode(sustainPin, INPUT_PULLUP);
  
  // set pullup in switches
  for (pinNumber = 5; pinNumber < 9; pinNumber++) {
    pinMode(pinNumber, INPUT_PULLUP);
  }
  
  // set LED pins as outputs
  for (pinNumber = 9; pinNumber < 13; pinNumber++) {
    pinMode(pinNumber, OUTPUT);
  }
}

void loop() {
  for (i = 0; i < 4; i++) {
    // read switches
    if (switches[i].update() && switches[i].fallingEdge()) {
      activeKnob = activeKnob != i ? i : -1;
    }
    
    // turn active knob LED on
    digitalWrite(9 + i, activeKnob == i ? HIGH : LOW);
  }

  // read expression pedal
  int expressionValue = analogRead(A0) / 8;
  if ((expressionValue != previousExpressionValue) && (activeKnob != -1)) {
    usbMIDI.sendControlChange(expressionCC[activeKnob], expressionValue, midiChannel);
    previousExpressionValue = expressionValue;
  }

  // read sustain pedal
  if (sustainPedal.update()) {
    // signal goes HIGH when pedal is pressed
    if (sustainPedal.risingEdge()) {
      usbMIDI.sendControlChange(sustainCC, 127, midiChannel);
    } else if (sustainPedal.fallingEdge()) {
      usbMIDI.sendControlChange(sustainCC, 0, midiChannel);
    }
  }

  // discard incoming MIDI messages
  while (usbMIDI.read()) {}
  delay(5);
}
