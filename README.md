#DCAA – Digitally Controlled Analog Architecture

##Overview
What is the DCAA?

The Digitally Controlled Analog Architecture (DCAA) is a circuit architecture designed to control analog signals digitally without negatively affecting their signal integrity.

By using digital control, parameter values can be stored, recalled, and controlled remotely. The DCAA can replace conventional potentiometers and switches while maintaining the analog signal path.

Instead of conventional switching, the architecture uses signal relays. Differential light-dependent resistors (LDRs) are used to emulate potentiometers.

##Applications
The DCAA is suitable for analog circuits that require the ability to store and recall settings or to be controlled remotely.
It was primarily developed for guitar effect pedals and preamps but can also be used in general analog audio-frequency (AF) circuits.

##Features
2 analog control channels using LDRs for potentiometer emulation
2 relay channels with changeover relays
Galvanic isolation for all channels
3 memory slots for storing and recalling previously saved settings
USB-MIDI and serial MIDI interfaces for remote control of all functions
Inputs
2 analog inputs for potentiometers
2 digital inputs for switches
4 digital inputs for push buttons used for preset recall
Outputs
2 digital outputs for status LEDs

Serial transmission of current program variables via USB for advanced users:
Baud rate: 9600
Data format: 8N1
Functional Description

##Startup
The DCAA starts automatically when a 5 V supply voltage is applied or when the USB cable is connected.
After startup, Preset 0 (Manual/Remote Mode) is activated. In this mode, the potentiometer values and switch positions are passed directly to the corresponding outputs.

##Manual and Remote Control
In Manual/Remote Mode, incoming MIDI values are forwarded to the corresponding outputs.
A MIDI-controlled value remains active until it is changed by either:
Adjusting the corresponding potentiometer
Receiving a new MIDI value

##Preset Recall
Presets 1 and 2 can be recalled using the corresponding preset buttons. The previously stored values are then applied to the outputs.
In preset mode, changes made using the potentiometers or MIDI commands are not applied directly to the outputs.
Presets can also be recalled via MIDI. Preset 3 is only accessible via MIDI.
See the MIDI Assignment Table for details.

##Saving Presets
To overwrite a preset memory slot:
Recall the preset you want to modify.
Set the desired values using the potentiometers and switches.
Press and hold the STORE button for 3 seconds.
After the settings have been stored successfully, the status LED lights up.

##Limitations and Electrical Specifications
The DCAA outputs provide galvanic isolation rated up to approximately 1000 V.
However, the maximum analog-side voltage must not exceed the following limits:

Maximum total voltage: 60 V DC
Maximum bipolar voltage: −30 V to +30 V

Operation at higher analog voltages is possible, but requires different types of relays and LDRs.

##Power Supply
Supply voltage: 5 V DC maximum
Maximum current consumption: approximately 250 mA
Correct supply polarity must be observed

Warning: Do not exceed the specified supply voltage or analog-side voltage limits.

##MIDI Assignment Table
Only MIDI commands received on MIDI Channel 1 are processed.

Output    MIDI Command	  MIDI Number	MIDI Value / Description
LDR 1     Control Change  7 (Volume)	0–127
LDR 2     Control Change  8	0–127
Relay 1	  Control Change  68	See relay control note below
Relay 2	  Control Change  69	See relay control note below
Preset 0	Program Change  0 (Manual)	—
Preset 1	Program Change  1	—
Preset 2	Program Change  2	—
Preset 3	Program Change  3	—

#Relay Control
The original relay assignment specifies the following switching thresholds:
Values greater than or equal to 100: ON
Values less than or equal to 99: OFF

Note: Both relays are listed with the same MIDI Control Change number (CC 68). Verify the intended relay addressing mechanism in the firmware documentation if the relays must be controlled independently.

##Credits

Author: Mike Utz
Project: UTZbox
Date: January 2026
