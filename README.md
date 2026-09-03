# Arduino RC Car — Project Resources

## Project Overview

This repository is the companion resource for an ESP32 RC car project shared on YouTube. It contains the ESP32 control program and 3D-printable STL files. A wiring diagram, parts list, photos, and detailed assembly instructions are still being prepared.

Any information that has not yet been confirmed is clearly marked **TODO** rather than filled in with untested details.

## Completed Image

> **TODO:** Add a photo of the completed RC car to the [`images/`](images/) folder, then display it here.

## YouTube Video

> **TODO:** Add the YouTube video title, thumbnail, and link after the video is published.

## Features

The included ESP32 program provides:

- A self-hosted Wi-Fi access point and browser-based controller
- Proportional forward and reverse speed control
- Proportional left and right steering control
- An emergency-stop button
- A 400 ms drive-command watchdog that stops the motors if commands are interrupted

## Parts List

The parts list will be available in the [`parts/`](parts/) folder.

> **TODO:** Add the exact parts and quantities after they have been confirmed.

| Part | Quantity | Notes | Link (optional) |
| --- | ---: | --- | --- |
| TODO | TODO | TODO | TODO |

Do not order parts from this table until the confirmed list has been added.

## Arduino Code

The ESP32 sketch is available at [`code/esp32_rccar/esp32_rccar.ino`](code/esp32_rccar/esp32_rccar.ino). See [`code/README.md`](code/README.md) for dependencies, pin assignments, upload instructions, and connection details.

The sketch has been successfully compiled for the generic ESP32 development board with ESP32 Arduino core 3.3.8 and ESP32Servo 3.2.1.

## STL Files

The 19 provided 3D-printable files are available in the [`stl/`](stl/) folder. See [`stl/README.md`](stl/README.md) for the complete file list.

> **TODO:** Add confirmed print settings, including material, orientation, supports, layer height, and infill where relevant.

## Wiring Diagram

The wiring diagram and related images will be placed in the [`wiring/`](wiring/) folder.

> **TODO:** Add a verified wiring diagram and a beginner-friendly connection table. Check all connections before powering the car.

## How to Build

Detailed assembly instructions will be added here after the design and build order have been verified.

1. **Prepare the parts** — TODO: Add the confirmed parts checklist.
2. **Print the components** — Download the files from [`stl/`](stl/). TODO: Add tested print settings and post-processing instructions.
3. **Assemble the chassis** — TODO: Add the verified assembly order and photos.
4. **Connect the electronics** — TODO: Add step-by-step instructions that match the final wiring diagram.
5. **Upload the code** — Follow the instructions in [`code/README.md`](code/README.md).
6. **Test the car** — TODO: Add a safe first-start procedure and functional checks.

## Common Problems

> **TODO:** Add solutions after the completed build has been tested. Include the exact symptoms, likely causes, and safe checks for each issue.

Suggested topics to document:

- The Arduino code does not upload
- The car does not power on
- A motor does not move or moves in the wrong direction
- Steering or remote control does not respond
- A printed part does not fit

These are troubleshooting categories only; project-specific solutions have not yet been added.

## Need Help?

If you're building this project, or another electronics or robotics project, and something isn't working as expected, feel free to contact me.

Instagram: [@takeru_robot](https://www.instagram.com/takeru_robot/)

I'm currently trying out a free, text-based troubleshooting session for makers. If you get stuck, feel free to reach out and tell me what you're building, what you expected to happen, and what happened instead.

## Repository Structure

```text
code/     Arduino code and setup notes
stl/      3D-printable model files and print settings
wiring/   Wiring diagrams and connection information
images/   Completed-build and assembly photos
parts/    Parts list and sourcing notes
```

## Project Status

The ESP32 program and STL files are available. The wiring diagram, parts list, photos, print settings, and full build instructions remain in progress. Sections marked **TODO** will be updated as those resources are verified.
