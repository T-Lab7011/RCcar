# ESP32 RC Car

This repository contains the public files for an ESP32-controlled RC car. Connect a phone or computer to the ESP32's Wi-Fi network to control forward and reverse motion and steering from a web browser.

## Contents

- [`code/`](code/) — ESP32 program and setup instructions
- [`stl/`](stl/) — 19 STL files for 3D printing

## Features

- ESP32-hosted Wi-Fi access point
- Browser-based control from a phone or computer
- Proportional forward and reverse speed control
- Proportional left and right steering control
- Emergency-stop button
- Motor safety stop if drive commands are interrupted for more than 400 ms

## Program

Sketch: [`code/esp32_rccar/esp32_rccar.ino`](code/esp32_rccar/esp32_rccar.ino)

### Requirements

- ESP32-compatible board
- Arduino IDE or Arduino CLI
- **esp32 by Espressif Systems** board package
- **ESP32Servo** library

The sketch has been compiled successfully with ESP32 Arduino core 3.3.8, ESP32Servo 3.2.1, and the generic **ESP32 Dev Module** board definition.

See [`code/README.md`](code/README.md) for detailed setup instructions and pin assignments.

### Connecting to the Car

1. Upload the program to the ESP32.
2. Connect a phone or computer to the Wi-Fi network shown below.
3. Open `http://192.168.4.1/` in a web browser.

| Setting | Default value |
| --- | --- |
| Network name (SSID) | `ESP32_RC_CAR` |
| Password | `12345678` |

## STL Files

The 19 STL files are available in [`stl/`](stl/). See [`stl/README.md`](stl/README.md) for the complete file list.

Print orientation, supports, layer height, infill, and other print settings are not included. Check each model in your slicer before printing.

## Safety Notes

- Raise the drive wheels off the ground for the first test.
- Verify the motor driver, servo power, grounds, and supply voltage before switching the car on.
- The communication watchdog is an additional safeguard and is not a substitute for testing the car in a safe environment.

## Contact

Instagram: [@takeru_robot](https://www.instagram.com/takeru_robot/)
