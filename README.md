# MiniSat

## Basic design:

We want to receive data from our from our 'ground station' over a 433MHz signal. This data is meant to control our payload (a 7 segment display).

Using our radio receiver module, our Raspberry pi on-board computer, and some code, we can receive the data and display it on the 7 segment display.

As we want to be continously receiving data, we will use both cores on the Raspberry Pi Pico. One core will be responsible for receiving and processing the incoming data, while the other core will be responsible for displaying the data on the 7 segment display.

The main code will be written in C++ and will be compiled using the Arduino IDE. The initial code can be found in the 'absatee_receiver_pico' folder.

## Tasks

1. Install Arduino IDE: https://www.arduino.cc/en/software
2. Install pico board manager: https://github.com/earlephilhower/arduino-pico
File > Preferences > Additional Boards Manager URLs: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
Hit OK
Tools > Board > Boards Manager
Search for "Raspberry Pi Pico/RP2040/RP2350" and install
3. Clone repository
4. Build attenna
5. Build breadboard (refer to schematic in this repository)
6. Write the code for the core 0 setup then test it
7. Write the code for the core 1 setup and core 0 loop then test it

## Antenna Design

There are two options for antenna design:

### 1. 1/4 Wave Monopole

This is the simplest antenna design. It is a straight wire that is 1/4 the wavelength of the signal you are transmitting. For the 433MHz signal, the wavelength is 69.2cm. Therefore, the antenna should be 17.3cm long.

### 2. Loaded Coil Antenna

A little more complicated, you can refer to the following guide: https://www.instructables.com/433-MHz-Coil-loaded-antenna/

## Documentation

Raspberry Pi Pico
https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf

Arduino Pico Core
https://arduino-pico.readthedocs.io/en/latest/

LSHD-5601 7 Segment Display
https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/1060/LSHD-5601.pdf

CAUTION: Middle pins on each side are VCC +5V anode. Pi PICO max current through gpio is ~50mA for all pins. 12mA per pin. To prevent any damage, we need a 500Ohm resistor from VCC to anode to limit current to 10mA. This can be done by putting one resistor on each side of the display (1kOhm) or one resistor on one side of the display (500Ohm) or any other combination of resistors that add up to 500Ohm.

## Troubleshooting

If your Arduino IDE gets stuck on "Downloading index: package_rp2040_index.json", there is a bug where this will occur if your serial monitor is open. Close the serial monitor and it should complete.
