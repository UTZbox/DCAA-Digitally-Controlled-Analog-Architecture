**The DCAA – Digitally Controlled Analog Architecture**

### What is the DCAA and how does it work:
The DCAA is a circuit architecture designed to control analog signals without negatively aﬀecting
them. Control is performed digitally, which allows values to be stored and remotely controlled.
The circuit can replace potentiometers or switches. Instead of conventional switching, signal
relays are used. Diﬀerential LDRs are used in place of potentiometers.

### Applications:
The DCAA can be used wherever analog circuits—especially in the audio domain—require the
ability to store and recall settings, or to be remotely controlled. It was primarily developed for
guitar eﬀect pedals and preamps, but it can also be used for general analog low-frequency (AF/
audio) circuits.

### Features:
* Two channels for potentiometer emulation (LDR)
* Two channels with changeover relays
* All channels are galvanically isolated
* Three memory slots for storing and recalling previously saved values
* USB-MIDI and serial MIDI interfaces for remote control of all functions
* Inputs:
* Two analog inputs (potentiometers)
* Two digital inputs (switches)
* Four digital inputs (push buttons) for preset recall
* Outputs:
* Two digital outputs (LEDs) for status indication
* For advanced users: current program variables are transmitted via USB
as serial data (9600 baud, 8N1)

### Functional Description:
The circuit starts automatically when a 5 V supply voltage is applied or when the USB cable is
connected. Preset Channel 0 (Manual/Remote Mode) is activated. In this mode, the values set by
the potentiometers and the switch positions are passed directly to the outputs.
If a value is received via MIDI, it is forwarded to the corresponding output. This value remains
active until it is changed either by adjusting a potentiometer or by receiving another MIDI value.
Preset buttons 1 and 2 recall the previously stored values and send them to the outputs. In this
mode, changes made via potentiometers or MIDI are not applied directly. Presets can also be
recalled via MIDI. Preset Channel 3 can only be recalled via MIDI. Refer to the table below for
assignments.
To overwrite a memory slot, first recall the preset to be changed. Then set the desired values
using the potentiometers and switches. After that, press and hold the STORE button for 3
seconds. Upon successful storage, the status LED lights up.

### Limitations:
The outputs of the circuit are galvanically isolated up to approximately 1000 V. However, the
maximum analog voltage must not exceed a total of 60 VDC or −30 V / +30 V. Operation with
higher voltages on the analog side is possible, but in this case diﬀerent types of relays and LDRs
must be used.
The supply voltage of the circuit itself must not exceed 5 VDC. Correct polarity must be observed.
Maximum current consumption is approximately 250 mA.


### MIDI Assignment Table:
Only commands on MIDI Channel 1 are received and processed.

Output MIDI Command MIDI No. MIDI Value
LDR 1 ControlChange 7 (Volume) 0-128
LDR 2 ControlChange 8 (notSpec) 0-128
Relais 1 ControlChange 68 (notSpec) >100 = ON // <99 = OFF
Relais 2 ControlChange 69 (notSpec) >100 = ON // <99 = OFF
Preset 0 ProgrammChange 0 (Manual) -
Preset 1 ProgrammChange 1 (Preset 1 -
Preset 2 ProgrammChange 2 (Preset 2) -
Preset 3 ProgrammChange 3 (Preset 2) -

###Credits
Author: Mike Utz
Project: UTZbox
Date: January 2026
