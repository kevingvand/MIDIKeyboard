#include "MIDIUSB.h"
#include "PitchToNote.h"

byte rows[] = {2, 3, 4, 5, 6, 7, 8, 9};
const int rowCount = sizeof(rows)/sizeof(rows[0]);

byte cols[] = {19, 18, 15, 14, 16, 10};
const int colCount = sizeof(cols)/sizeof(cols[0]);

byte previousKeys[colCount][rowCount];
byte pressedKeys[colCount][rowCount];

byte notes[colCount][rowCount] = {
  {pitchD5,pitchD5b,0,0,0,0,0,0},
  {0,0,0,0,0,pitchF5,pitchE5,pitchE5b},
  {pitchC5,pitchB4,pitchB4b,pitchA4,pitchA4b,pitchG4,pitchG4b,pitchF4},
  {pitchE4,pitchE4b,pitchD4,pitchD4b,pitchC4,pitchB3,pitchB3b,pitchA3},
  {pitchA3b,pitchG3,pitchG3b,pitchF3,pitchE3,pitchE3b,pitchD3,pitchD3b},
  {pitchC3,pitchB2,pitchB2b,pitchA2,pitchA2b,pitchG2,pitchG2b,pitchF2},
};

void setup() {
    Serial.begin(115200);
 
    for(int x=0; x<rowCount; x++) {
        Serial.print(rows[x]); Serial.println(" as input");
        pinMode(rows[x], INPUT);
    }
 
    for (int x=0; x<colCount; x++) {
        Serial.print(cols[x]); Serial.println(" as input-pullup");
        pinMode(cols[x], INPUT_PULLUP);
    }
         
}

void readMatrix() {
    // iterate the columns
    for (int colIndex=0; colIndex < colCount; colIndex++) {
        // col: set to output to low
        byte curCol = cols[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);
 
        // row: interate through the rows
        for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
            byte rowCol = rows[rowIndex];
            previousKeys[colIndex][rowIndex] = pressedKeys[colIndex][rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            pressedKeys[colIndex][rowIndex] = digitalRead(rowCol);
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }
}
 
void printMatrix() {
    for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
        if (rowIndex < 10)
            Serial.print(F("0"));
        Serial.print(rowIndex); Serial.print(F(": "));
 
        for (int colIndex=0; colIndex < colCount; colIndex++) {  
            Serial.print(pressedKeys[colIndex][rowIndex]);
            if (colIndex < colCount)
                Serial.print(F(", "));
        }   
        Serial.println("");
    }
    Serial.println("");
}

void sendMatrix() {
    for (int rowIndex=0; rowIndex < rowCount; rowIndex++) { 
        for (int colIndex=0; colIndex < colCount; colIndex++) {  

            if(pressedKeys[colIndex][rowIndex] != previousKeys[colIndex][rowIndex]) {
              if(pressedKeys[colIndex][rowIndex] == LOW) {
                previousKeys[colIndex][rowIndex] = LOW;
                noteOn(0, notes[colIndex][rowIndex], 800);
                MidiUSB.flush();  
              } else {
                previousKeys[colIndex][rowIndex] = HIGH;
                noteOff(0, notes[colIndex][rowIndex], 0);
                MidiUSB.flush();  
              }
            }
          
        }   
    }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void loop() {
    readMatrix();
    sendMatrix();
    //printMatrix();
    //delay(1000);
}
