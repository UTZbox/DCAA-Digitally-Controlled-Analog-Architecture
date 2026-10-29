/* Digital2Analog Poti Interface
Author:       Michael Utz, Dachslernstrasse 74, 8048 Zurich
Date:         11. November 2024
Description:
The Device reads the Poti Value on its analog inputs
Also some MIDI and USB-Midi inputs.
The Values will be stored in different Presets.
Over a preset switch the dedicated preset can be choosen.
The Values of the dedicated values odfthe preset will then be sent to the PWM Output
At the PWM Outputs are OpAmps and LDR wich represents the orriginal value of the Poti.
The advantage of the systems is that the digitale part is fully galvanic isolated and do not affect the analog part accept the LDR resitance value.
This way it is possible to store and switch between different values.

Version:      0.1 Prototype with 2 In/Output
              0.2 added Store Button and reverse pwm outputs
              0.3 added read n store from eeprom and build all presets
              0.4 changed the modeSwitch to 3 inputs and to momentary
                  bugfix switching device mode between poti and midi
                  added 2 Led for displaying deviceMode
                  added the input and output for boostSwitch / boostOut
              0.5 added full function of Overdrive and Boost Switch, Output and Preset
              0.6 Bug Fixing and addimg scalling of analog Outputs
              0.7 Bug Fixing, add Preset 3 to LED Output
              
Compiling:  Choose Board: = Tennsy 4.0, Keyboard: = US english, Optimize: = Faster, CPU Speed: = 600 MHz, USB Type: = Serial+Midi
*/

// include libraries
#include <MIDI.h>
#include <EEPROM.h>

// Created and binds the MIDI interface to the default hardware Serial port
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// define input and output pins
// Pin 0 is reserved for MIDI In
// digital inputs:
int modeSwitch1    = 1;
int modeSwitch2    = 2;
int modeSwitch3    = 3;
int storeButton    = 4;
int ovrDrvSwitch   = 5;
int boostSwitch    = 6;

// analog inputs
int volumePot      = A0;
int gainPot        = A1;

// digital outputs:
int statusLed      = 13;
int ovrDrvRel      = 20;
int boostRel       = 21;
int modeLed1       = 22;
int modeLed2       = 23;   

//analog outputs:
int volumePwm      = 9;
int volumePwmInv   = 10;
int gainPwm        = 11;
int gainPwmInv     = 12;

// define global variables
volatile int midiChan       = 1;   // Global Midi Channel
volatile int midiCcChan     = 0;
volatile int midiCcNr       = 0;
volatile int midiCcVal      = 0;
volatile int midiLastCcVal  = 0;
volatile int midiPcChan     = 0;
volatile int midiPcPrg      = 0;
volatile int midiNewPrgMsg  = 0;

volatile int volume         = 0;
volatile int lastVolume     = 0;
volatile int gain           = 0;
volatile int lastGain       = 0;
volatile int ovrDrv         = 0;
volatile int lastOvrDrv     = 0;
volatile int boost          = 0;
volatile int lastBoost      = 0;

volatile int volumeOut      = 0;
volatile int gainOut        = 0;
volatile int ovrDrvOut      = 0;
volatile int boostOut       = 0;

volatile int scalledVolumeOut = 0;
volatile int scalledGainOut = 0;

volatile int deviceMode     = 0;
volatile int lastDeviceMode = 0;

bool storeReady             = false;
unsigned long storePresetDelay    = 3000;
unsigned long lastStorePressed    = 0;


// definde arrays to store the values in presets
int preset_0[4] = {0, 0, 0, 0};
int preset_1[4] = {0, 0, 0, 0};
int preset_2[4] = {0, 0, 0, 0};

// timers
elapsedMillis inpUpdateTime;
elapsedMillis sinceLastPrint;
elapsedMillis sinceStorePress;

// functions
//! Called when we receive a Pogramm Change event
void myProgramChange(byte channel, byte program) {
  midiPcChan = channel;
  midiPcPrg = program;
  midiNewPrgMsg = 1;
}

//! Called when we recieve a ControlChange event
void OnControlChange(byte channel, byte control, byte value) {
  // Store the incoming values to global variables
  midiCcChan = channel;
  midiCcNr = control;
  midiCcVal = value;
}

// add hardware midi handling if needed ------


// setup routine -------------------------------------------------------------
void setup() {
  //setup input and output pin functions
  pinMode(modeSwitch1, INPUT_PULLUP);
  pinMode(modeSwitch2, INPUT_PULLUP);
  pinMode(modeSwitch3, INPUT_PULLUP);
  pinMode(storeButton, INPUT_PULLUP);
  pinMode(ovrDrvSwitch, INPUT_PULLUP);
  pinMode(boostSwitch, INPUT_PULLUP);
  pinMode(modeLed1, OUTPUT);
  pinMode(modeLed2, OUTPUT);
  pinMode(ovrDrvRel, OUTPUT);
  pinMode(boostRel, OUTPUT);
  pinMode(statusLed, OUTPUT);

  // setup analog resolution and frequency of pwm pins:
  analogReadRes(10); // Sets the Anaalog input to 10 Bot 0 -1023
  analogWriteResolution(10); // Sets the PWM Output to 10 Bit 0-1023
  analogWriteFrequency(volumeOut, 100000); // Teensy 3.0 pin 3 also changes to 375 kHz
  analogWriteFrequency(gainOut, 100000); // Teensy 3.0 pin 3 also changes to 375 kHz

  // call function for midi handling
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleProgramChange(myProgramChange);

  //call Funcrtion to read the eeprom values and store it into ram
  preset_0[0] = EEPROM.read(0) * 4;
  preset_0[1] = EEPROM.read(1) * 4;
  preset_0[2] = EEPROM.read(2);
  preset_0[3] = EEPROM.read(3);
  preset_1[0] = EEPROM.read(4) * 4;
  preset_1[1] = EEPROM.read(5) * 4;
  preset_1[2] = EEPROM.read(6);
  preset_1[3] = EEPROM.read(7);
  preset_2[0] = EEPROM.read(8) * 4;
  preset_2[1] = EEPROM.read(9) * 4;
  preset_2[2] = EEPROM.read(10);
  preset_2[3] = EEPROM.read(11);



  // Start the Hardware MIDI Output
  // MIDI.begin(midiChan);

  // Turn Status LED on and of to show that the device is ready
  digitalWrite(statusLed, HIGH);
  delay(500);
  digitalWrite(statusLed, LOW);
}

// loop routine ---------------------------------------------------------------
void loop() {
  // read midi input at defined channel
  usbMIDI.read(midiChan);
  // MIDI.read();

  //read the analog potis and stores its values to global variables
  if (inpUpdateTime >= 50){
  volume = analogRead(volumePot);
  gain = analogRead(gainPot);
    if (digitalRead(ovrDrvSwitch) == LOW){
      ovrDrv = 1;
    }
    if (digitalRead(ovrDrvSwitch) == HIGH){
      ovrDrv = 0;
    }
    if (digitalRead(boostSwitch) == LOW){
      boost = 1;
    }
    if (digitalRead(boostSwitch) == HIGH){
      boost = 0;
    }
  inpUpdateTime = 0;
  }

  // handling mode switching by the hardware switch
  if (digitalRead(modeSwitch1) == LOW && digitalRead(modeSwitch2) == HIGH && digitalRead(modeSwitch3) == HIGH){
    deviceMode = 0;
    }
  else if (digitalRead(modeSwitch1) == HIGH && digitalRead(modeSwitch2) == LOW && digitalRead(modeSwitch3) == HIGH){
    deviceMode = 1;
    }
  else if (digitalRead(modeSwitch1) == HIGH && digitalRead(modeSwitch2) == HIGH && digitalRead(modeSwitch3) == LOW){
    deviceMode = 2;
    }

  // Handling mode switching by midi
  if (midiNewPrgMsg == 1 && midiPcPrg == 3){
    deviceMode = 3;
    midiNewPrgMsg = 0;
  }
  else if (midiNewPrgMsg == 1 && midiPcPrg == 2){
    deviceMode = 2;
    midiNewPrgMsg = 0;
  }
  else if (midiNewPrgMsg == 1 && midiPcPrg == 1){
    deviceMode = 1;
    midiNewPrgMsg = 0;
  }
  else if (midiNewPrgMsg == 1 && midiPcPrg == 0){
    deviceMode = 0;
    midiNewPrgMsg = 0;
  }

  // read the devvice (mode) and set the desired ouput values
  // preset 0 = actual analog or midi inputs goes to output
  if (deviceMode == 0){
    //Set the anaolg outputs
    if (volume >= lastVolume +4 || volume <= lastVolume -4){
      lastVolume = volume;
      volumeOut = volume;
    }
    if (gain >= lastGain +4 || gain <= lastGain -4 ){
      lastGain = gain;
      gainOut = gain;
    }
    // Set the digital outputs
    if (ovrDrv != lastOvrDrv){
      lastOvrDrv = ovrDrv;
      ovrDrvOut = ovrDrv;
    }
    if (boost != lastBoost){
      lastBoost = boost;
      boostOut = boost;
    }
    // Function to set the values when swicthing back from presets
    if (lastDeviceMode != 0){
      lastVolume = volume;
      volumeOut = volume;
      lastGain = gain;
      gainOut = gain;
      ovrDrvOut = ovrDrv;
      boostOut = boost;
      lastDeviceMode = 0;
    }
    // Functions to change the value only if a new midi message has sent 
    if (midiCcVal != midiLastCcVal && midiCcNr == 7){
      midiLastCcVal = midiCcVal;
      volumeOut = midiCcVal * 8;
      }
    
    if (midiCcVal != midiLastCcVal && midiCcNr == 8){
      midiLastCcVal = midiCcVal;
      gainOut = midiCcVal * 8;
      }

    if (midiCcVal != midiLastCcVal && midiCcNr == 68){
      midiLastCcVal = midiCcVal;
      if (midiCcVal >= 100){
        ovrDrvOut = 1;
        }
      if (midiCcVal <= 99){
        ovrDrvOut = 0;
        }
    }

    if (midiCcVal != midiLastCcVal && midiCcNr == 69){
      midiLastCcVal = midiCcVal;
      if (midiCcVal >= 100){
        boostOut = 1;
        }
      if (midiCcVal <= 99){
        boostOut = 0;
        }
    }

  }

  if (deviceMode == 1){
    volumeOut = preset_0[0];
    gainOut = preset_0[1];
    ovrDrvOut = preset_0[2];
    boostOut = preset_0[3];
    lastDeviceMode = 1;
  }

  if (deviceMode == 2){
    volumeOut = preset_1[0];
    gainOut = preset_1[1];
    ovrDrvOut = preset_1[2];
    boostOut = preset_1[3];
    lastDeviceMode = 2;
  }

  if (deviceMode == 3){
    volumeOut = preset_2[0];
    gainOut = preset_2[1];
    ovrDrvOut = preset_2[2];
    boostOut = preset_2[3];
    lastDeviceMode = 3;
  }



  // scalling the Output because of saturation of LDRs
  scalledVolumeOut = map(volumeOut, 0, 1023, 320, 720);
  scalledGainOut = map(gainOut, 0, 1023, 320, 720);

  // set and write the analog values to the output
  analogWrite(volumePwm, scalledVolumeOut);
  analogWrite(volumePwmInv, 1023 - scalledVolumeOut);
  analogWrite(gainPwm, scalledGainOut);
  analogWrite(gainPwmInv, 1023 - scalledGainOut);

  // set and write the digital states to the output
  digitalWrite(ovrDrvRel, ovrDrvOut);
  digitalWrite(boostRel, boostOut);

  if (deviceMode == 0){
    digitalWrite(modeLed1, LOW);
    digitalWrite(modeLed2, LOW);
  }

  else if (deviceMode == 1){
    digitalWrite(modeLed1, HIGH);
    digitalWrite(modeLed2, LOW);
  }

  else if (deviceMode == 2){
    digitalWrite(modeLed1, LOW);
    digitalWrite(modeLed2, HIGH);
  }

  else if (deviceMode == 3){
    digitalWrite(modeLed1, HIGH);
    digitalWrite(modeLed2, HIGH);
  }
  
  // handle the store update button
  if (digitalRead(storeButton) == LOW && storeReady == false){
    lastStorePressed = sinceStorePress;
    storeReady = true;
  }
  else if (digitalRead(storeButton) == HIGH) {
    storeReady = false;
  }

  if (storeReady == true) {
    if (sinceStorePress - lastStorePressed >= storePresetDelay){
      digitalWrite(statusLed, HIGH);
      if (deviceMode == 1){
         preset_0[0] = volume;
         preset_0[1] = gain;
         preset_0[2] = ovrDrv;
         preset_0[3] = boost;
         EEPROM.write(0, volume / 4);
         EEPROM.write(1, gain / 4);
         EEPROM.write(2, ovrDrv);
         EEPROM.write(3, boost);
      }
      if (deviceMode == 2){
         preset_1[0] = volume;
         preset_1[1] = gain;
         preset_1[2] = ovrDrv;
         preset_1[3] = boost;
         EEPROM.write(4, volume / 4);
         EEPROM.write(5, gain / 4);
         EEPROM.write(6, ovrDrv);
         EEPROM.write(7, boost);
      }
      if (deviceMode == 3){
         preset_2[0] = volume;
         preset_2[1] = gain;
         preset_2[2] = ovrDrv;
         preset_2[3] = boost;
         EEPROM.write(8, volume / 4);
         EEPROM.write(9, gain / 4);
         EEPROM.write(10, ovrDrv);
         EEPROM.write(11, boost);
      }
      storeReady = false;
      Serial.println("stored");
      delay(1000);
      digitalWrite(statusLed, LOW);
    }
  }

  // debug and status
  if (sinceLastPrint> 500){
    Serial.print("midiNewPrgMsg     ");
    Serial.println(midiNewPrgMsg);
    Serial.print("deviceMode        ");
    Serial.println(deviceMode);
    Serial.print("midiCcNr          ");
    Serial.println(midiCcNr);
    Serial.print("midiCcVal         ");
    Serial.println(midiCcVal);
    Serial.print("Volume            ");
    Serial.println(volumeOut);    
    Serial.print("scalledVolume     ");
    Serial.println(scalledVolumeOut);
    Serial.print("scalledVolumeInv     ");
    Serial.println(1023 - scalledVolumeOut);
    Serial.print("Gain              ");
    Serial.println(gainOut);
    Serial.print("scalledGain       ");
    Serial.println(scalledGainOut);
    Serial.print("scalledGainInv            ");
    Serial.println(1023 - scalledGainOut);
    Serial.print("ovrDrvOut         ");
    Serial.println(ovrDrvOut);
    Serial.print("boostOut          ");
    Serial.println(boostOut);
    Serial.print("storeReady        ");
    Serial.println(storeReady);
    Serial.print("preset_0          ");
    Serial.print(preset_0[0]);
    Serial.print(" / ");
    Serial.print(preset_0[1]);
    Serial.print(" / ");
    Serial.print(preset_0[2]);
    Serial.print(" / ");
    Serial.println(preset_0[3]);
    Serial.print("EEPROM            ");
    Serial.print(EEPROM.read(0) * 4);
    Serial.print(" / ");
    Serial.print(EEPROM.read(1) * 4);
    Serial.print(" / ");
    Serial.print(EEPROM.read(2));
    Serial.print(" / ");
    Serial.println(EEPROM.read(3));
    sinceLastPrint = 0;
  }
}