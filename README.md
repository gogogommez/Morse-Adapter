# Morse-Adapter

USB-HID adapter for connecting an iambic Morse paddle to a computer or mobile device using a Raspberry Pi Pico / RP2040.

Developed by **Roland Schaal**.

## Features

- Morse Mania mode: left paddle = `a`, right paddle = `s`
- VBand / Morse Invaders mode: left paddle = `[`, right paddle = `]`
- Supports simultaneous paddle input (squeezing)
- Hold both paddles for **15 seconds** to switch modes
- Selected mode is stored in EEPROM
- LED indication: 1 blink = Morse Mania, 2 blinks = VBand / Morse Invaders

## Wiring

| Paddle | Raspberry Pi Pico |
| --- | --- |
| Left | GP10 |
| Right | GP2 |
| Common | GND |

The GPIO inputs use the Pico's internal pull-up resistors.

## Arduino setup

Use the **Raspberry Pi Pico/RP2040 Arduino Core by Earle F. Philhower**.

In the Arduino IDE select:

`Tools -> USB Stack -> Adafruit TinyUSB`

The sketch uses:

- `Keyboard.h`
- `EEPROM.h`

## Firmware

The Arduino sketch is located in `Morse-Adapter.ino`.

## Hardware

The adapter uses a Raspberry Pi Pico, a 3.5 mm paddle jack and a 3D-printed enclosure.

Photos and 3D-print files will be added to this repository.

## Author

**Roland Schaal**

The Raspberry Pi Pico/RP2040 Arduino Core used by this project is developed by **Earle F. Philhower**.
