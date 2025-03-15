# Setting Up Your HeidelBridge

## Initial Programming Of The ESP32

Follow these steps to turn your ESP32 into a HeidelBridge. This has to be done only once. After that you can update your HeidelBridge via OTA-updates (see [here](#updating-your-heidelbridge)).

- Connect your ESP32 to the computer via USB.
- Visit the [HeidelBridge Web Flasher](https://borisbrock.github.io/HeidelBridge/) in a Chromium based web browser (Chromium, Brave, Vivaldi etc.). Firefox does currently **not** support the WebSerial standard.
- Connect to your ESP32.
- Flash the firmware to your ESP32.

### Fixing Connection Issues

If you cannot connect to your ESP32 via WebSerial, these hints might help:
- On Windows, make sure you have installed the serial port drivers for your ESP32. It should appear as a COM port in the Windows Device Manager.
- On Linux, make sure you have added your user to the required user groups (e.g. `sudo usermod -aG dialout $USER` for Ubuntu, `sudo usermod -aG uucp,lock $USER` for Arch)
- Sometimes you need to hold the boot-button and/or press the reset button for the flahing process to start.

## Configuring Your HeidelBridge

Once the firmware has been uploaded, the device will restart and open up a Wifi captive portal. Use your smartphone or laptop to connect to this access point. The SSID is `HeidelBridge Setup`.

You should then automatically be taken to the configuration web interface of the HeidelBridge. If this is not the case, open the IP address `4.3.2.1` manually in a web browser.

In the web interface you need to configure your home WiFi SSID and password. You can also enable MQTT and set up the MQTT connection. Once all settings have been entered, click *Apply and restart* to save the settings.

![Settings](/docs/img/Screenshot_Settings.png)

The HeidelBridge will restart and connect to your WiFi and MQTT server.

## Changing Your HeidelBridge Configuration

If you want to change the configuration of your HeidelBridge, you can open the web interface at any time by navigating to your HeidelBridge's IP address in a web browser.

## Updating Your HeidelBridge

To update the firmware on your HeidelBridge, download the latest firmware binary from the [GitHub releases](https://github.com/BorisBrock/HeidelBridge/releases).

Now open the update web interface of your heidelbridge in a web browser: `{ip}/update` (where {ip} is the IP address of your HeidelBridge).

Here you can upload the firmware binary and start the update.

![OTA](/docs/img/Screenshot_OTA.png)

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