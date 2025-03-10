# Setting Up Your HeidelBridge

## Initial Programming Of The ESP32

Follow these steps to turn your ESP32 into a HeidelBridge:

- Download the firmware binary from the [latest GitHub release](https://github.com/BorisBrock/HeidelBridge/releases)
- Connect your ESP32 to the computer via USB.
- Visit the [ESPWEBTOOL](https://esp.huhn.me/) in a Chromium based web browser (Chromium, Brave, Vivaldi etc.).
- Load the firmware binary to your ESP32.

## Configuring Your HeidelBridge

TODO: Captive portal...

## Updating Your HeidelBridge

TODO: GitHub release binary download, OTA flash {ip}/update

## Changing The HeidelBridge Configuration

TODO: Web Interface...

## Programming Your ESP32 Manually

To compile this project you will need to install VS Code and the PlatformIO extension. Both are available for free for Linux, MacOS and Windows.

Then follow these steps:

- Start by cloning or downloading this repository.
- Optional: change `board = ...` in platformio.ini to match the ESP32 board you are actually using.
- Compile the project.
- Build the file system image via the PlatformIO command palette.
- Now connect your ESP32 via USB.
- Upload the file system image.
- Upload the firmware.