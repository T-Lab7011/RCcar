# ESP32 RC Car Program

The [`esp32_rccar/esp32_rccar.ino`](esp32_rccar/esp32_rccar.ino) sketch turns an ESP32 into a Wi-Fi access point and hosts a browser-based RC car controller. It controls two motors through a DRV8833 motor driver and uses one servo for steering.

## Requirements

- ESP32-compatible board
- Arduino IDE or Arduino CLI
- **esp32 by Espressif Systems** board package (compiled successfully with version 3.3.8)
- **ESP32Servo** library (compiled successfully with version 3.2.1)

`WiFi.h` and `WebServer.h` are supplied by the ESP32 board package.

## Pin Assignments

| Function | ESP32 pin |
| --- | ---: |
| DRV8833 AIN1 | GPIO 25 |
| DRV8833 AIN2 | GPIO 33 |
| DRV8833 BIN1 | GPIO 27 |
| DRV8833 BIN2 | GPIO 14 |
| Steering servo signal | GPIO 32 |

The motor outputs use 20 kHz, 8-bit PWM. The servo is centered at 90 degrees and constrained from 40 to 150 degrees. Confirm that the pin assignments match your wiring before applying power.

## Upload and Use

1. Install the ESP32 board package using the Arduino IDE Boards Manager.
2. Install `ESP32Servo` using the Library Manager.
3. Open [`esp32_rccar/esp32_rccar.ino`](esp32_rccar/esp32_rccar.ino).
4. Select the board definition that matches your ESP32. The sketch has been compiled successfully using the generic **ESP32 Dev Module** definition.
5. Check the pin assignments and upload the sketch.
6. Connect a phone or computer to the Wi-Fi network shown below.
7. Open `http://192.168.4.1/` in a web browser.

## Default Wi-Fi Settings

| Setting | Default value |
| --- | --- |
| Network name (SSID) | `ESP32_RC_CAR` |
| Password | `12345678` |

The SSID and password can be changed using the `ssid` and `password` values near the top of the sketch.

## Safety Notes

- Raise the drive wheels off the ground for the first test.
- Verify the motor driver, servo power, grounds, and supply voltage before switching the car on.
- The motors stop if drive commands are interrupted for more than 400 ms, but this safeguard is not a substitute for testing the car in a safe environment.
