
# M5Paper Library

[![Arduino Compile](https://github.com/m5stack/M5EPD/actions/workflows/arduino-action-paper-compile.yml/badge.svg)](https://github.com/m5stack/M5EPD/actions/workflows/arduino-action-paper-compile.yml)
[![Arduino Lint](https://github.com/m5stack/M5EPD/actions/workflows/Arduino-Lint-Check.yml/badge.svg)](https://github.com/m5stack/M5EPD/actions/workflows/Arduino-Lint-Check.yml)
[![Clang Format](https://github.com/m5stack/M5EPD/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/m5stack/M5EPD/actions/workflows/clang-format-check.yml)


English | [中文](README_cn.md)

<img src="https://m5stack.oss-cn-shenzhen.aliyuncs.com/image/product_jpg/assets/img/product_pics/core/m5paper/m5paper_01.jpg" alt="m5paper_pic_01" width="400px">

## Description

**M5Paper** is M5Stacks latest core device with a touch enabled E-ink display. Powered by the ESP32-D0WDQ6-V3 this is our first device to integrate a super sized 540*960 @4.7" E-ink display，which supports 16 grayscale levels. The display is a GT911 capacitive touch screen，which supports two point touch and a variety of gesture controls . Compared to a regular LCD，E-ink displays are easier on the eyes, which makes them a great choice for reading or viewing for longer periods. Other benefits are the low power consumption and the ability to retain the image even if power to the display is terminated. Integrated in the CoreInk are an multi-function button for operation, SHT30 temperature and moisture sensor, physical buttons and an TF card port for data storage.

Additionally the FM24C02 internal eeprom chip(256KB-EEPROM)can be used to store vital data even when the device is off. A 1150mAh lipo battery keeps the device going for long periods and battery life can be further preserved by using the RTC(BM8563)to set the device into deep sleep and wake it up again when needed。Three HY2.0-4P expansion ports are included which allow for building complex projects using the existing sensors in the M5Stack ecosystem.

?>Warning: Do not expose to ultraviolet rays for a long time, otherwise it may cause irreversible damage to the ink screen.

## PinMap

**E-INK & TF Card**

Resolution：540*960
 
<table>
 <tr><td>ESP32 Chip</td><td>GPIO13</td><td>GPIO12</td><td>GPIO14</td><td>GPIO15</td><td>GPIO4</td></tr>
 <tr><td>IT8951</td><td>MISO</td><td>MOSI</td><td>SCK</td><td>CS</td><td>/</td></tr>
 <tr><td>TF Card</td><td>MISO</td><td>MOSI</td><td>SCK</td><td>/</td><td>CS</td></tr>
</table>


**Multi-function button**

<table>
 <tr><td>ESP32 Chip</td><td>GPIO37</td><td>GPIO38</td><td>GPIO39</td></tr>
 <tr><td>Multi-function button</td><td>Right</td><td>BTN/PWR</td><td>Left</td></tr>
</table>


**Internal I2C Connection**

<table>
 <tr><td>ESP32 Chip</td><td>GPIO21</td><td>GPIO22</td><td>GPIO36</td></tr>
 <tr><td>GT911</td><td>SDA</td><td>SCL</td><td>INT</td></tr>
 <tr><td>SHT30</td><td>SDA</td><td>SCL</td><td>/</td></tr>
 <tr><td>BM8563</td><td>SDA</td><td>SCL</td><td>/</td></tr>
 <tr><td>FM24C02</td><td>SDA</td><td>SCL</td><td>/</td></tr>
</table>

**USB Serial**

<table>
 <tr><td>ESP32 Chip</td><td>GPIO1</td><td>GPIO3</td></tr>
 <tr><td>CP2104</td><td>RXD</td><td>TXD</td></tr>
</table>

## M5Paper-HY2.0 4P Port

<table>
      <thead>
         <th>PORT</th>
         <th>PIN</th>
         <th>Protocol:</th>
      </thead>
      <tbody>
      <tr>
         <td>PORT.A</td>
         <td>G25,G32</td>
         <td>I2C</td>
      </tr>
      <tr>
         <td>PORT.B</td>
         <td>G26,G33</td>
         <td>DAC/ADC</td>
      </tr>
      <tr>
         <td>PORT.C</td>
         <td>G18,G19</td>
         <td>UART</td>
      </tr>
    </tbody>
</table>

## ESP32 ADC/DAC Mappable Pins

<table>
      <thead>
         <th>ADC1</th>
         <th>ADC2</th>
         <th>DAC1</th>
         <th>DAC2</th>
      </thead>
      <tbody>
      <tr>
         <td>8 Channel</td>
         <td>10 Channel</td>
         <td>2 Channel</td>
         <td>2 Channel</td>  
      </tr>
      <tr>
         <td>G32-39</td>
         <td>G0/2/4/12-15/25-27</td>
         <td>G25</td>
         <td>G26</td>
      </tr>
    </tbody>
</table>

For more info on specific pin functions refer to the official ESP32 Docs[ESP32 datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/esp32_datasheet_en.pdf)


## How to compile it

### For Windows

1. Install [ESP-IDF v4.4][esp-idf-windows-installer]. For further info, read: [ESP-IDF Getting Started for Windows][esp-idf-windows-setup]

   * Either the Online or Offline version shoud work
   * When asked which components to install, don't change anything. Default options are Ok.
   * When asked whether ESP can modify the system, answer "Yes"

2. Launch the "ESP-IDF v4.4 CMD" (type that in the Windows search box)

3. From the ESP-IDF cmd, clone the template

   ```sh
   git clone --recursive https://github.com/m5stack/M5EPD.git my_project
   ```

4. Compile it

    ```sh
    # Compile it
    cd my_project
    idf.py build

    # Flash + open debug terminal
    idf.py flash monitor
    ```

[esp-idf-windows-setup]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html
[esp-idf-windows-installer]: https://dl.espressif.com/dl/esp-idf/?idf=4.4

### For Linux / macOS

1. Requirements and permissions

    Install ESP-IDF dependencies (taken from [here][toolchain-deps]):

    ```sh
    # For Ubuntu / Debian
    sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
    ```

    And in case you don't have permissions to open `/dev/ttyUSB0`, do:
    (taken from [here][ttyusb0])

    ```sh
    # You MUST logout/login (or in some cases reboot Linux) after running this command
    sudo usermod -a -G dialout $USER
    ```

2. Install and setup ESP-IDF

    ```sh
    # Needs to be done just once
    # Clone the ESP-IDF git repo
    mkdir ~/esp && cd ~/esp
    git clone -b release/v4.4 --recursive https://github.com/espressif/esp-idf.git

    # Then install the toolchain
    cd ~/esp/esp-idf
    ./install.sh
    ```

3. Compile the template

    Clone the template:

    ```sh
    # Do it everytime you want to start a new project
    # Clone the template somewhere
    mkdir ~/src && cd ~/src
    git clone --recursive https://github.com/m5stack/M5EPD.git my_project
    ```

    Export the ESP-IDF environment variables in your shell:

    ```sh
    # Do it everytime you open a new shell
    # Optional: add it in your ~/.bashrc or ~/.profile
    source ~/esp/esp-idf/export.sh
    ```

    And finally compile and install your project.

    ```sh
    # Compile it
    cd ~/src/my_project
    idf.py build

    # Flash + open debug terminal
    idf.py flash monitor
    ```

[toolchain-deps]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-setup.html
[ttyusb0]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/establish-serial-connection.html#linux-dialout-group

## Using 3rd party Arduino libraries

To include 3rd party Arduino libraries in your project, you have to:

* Add them to the `components` folder.
* Add a file `component.mk` and/or `CMakeLists.txt` inside the component's folder

`component.mk` is needed if you use `make` to compile it. And `CMakeLists.txt` is needed if you use `idf.py` to compile it.

Let's use a real case as example:

### Example: Adding ESP32Servo

Suppose you want to use [ESP32Servo] project. The first thing to notice is that the source files are placed
in the `src` folder.  We have to create a `component.mk` and/or `CMakeLists.txt` files that tells the
ESP-IDF to look for the sources in the `src` folder.

Example:

```sh
# 1) We clone ESP32Servo into components folder
cd components
git clone https://github.com/madhephaestus/ESP32Servo.git
cd ESP32Servo
```

And now create create these files files inside `components/ESP32Servo` folder:

```sh
# 2) Create component.mk file
# Only needed if you use "make" to compile the project
# Copy & paste the following lines to the terminal:
cat << EOF > component.mk
COMPONENT_ADD_INCLUDEDIRS := src
COMPONENT_SRCDIRS := src
EOF
```

```sh
# 3) Create CMakeLists.txt file
# Only needed if you use "idf.py" to compile the project
# Copy & paste the following lines to the terminal:
cat << EOF > CMakeLists.txt
idf_component_register(SRC_DIRS "src"
                    INCLUDE_DIRS "src"
                    REQUIRES "arduino")
EOF
```

Finally, if you use `idf.py`, you have to update the dependencies in the `main/CMakeLists.txt`. E.g:

```sh
# Needed if you use "idf.py" to compile the project
cd main
edit CMakeLists.txt
```

...and append `ESP32Servo` to `REQUIRES`. The `main/CMakeLists.txt` should look like this:

```cmake
idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRES "${requires}" "ESP32Servo")
```

And that's it. Now you can include `ESP32Servo` from your code. E.g:

```cpp
// Add this include in your arduino_main.cpp file
#include <ESP32Servo.h>
```

[esp32servo]: https://github.com/madhephaestus/ESP32Servo.git

## IDE

Arduino IDE is not supported, but you can use [Visual Studio Code][vscode] + [ESP-IDF plugin][esp-idf-plugin].

You can do:

* All the regular Visual Studio Code regular features
* ...plus configure, build, flash and monitor your project
* ...and much more

![ide](https://lh3.googleusercontent.com/pw/AM-JKLUxjqUhU2tM-bKw3togS3gTkBdtmi40kqW2c2KieAybnD770I3pdaLnFU7a-sM7dUUGmcWpigvElc1fGo1J-5bJlVdbg8HOJZKbUXo6A_IqIvUGEK6GtwxqNy5EFJmijrBnB_aQhd_fi3GQnXZ1V7WYvw=-no)

Subjective opinion: VSCode + ESP-IDF plugin is muuuuuch better than Arduino IDE. Highly recommended!

[vscode]: https://code.visualstudio.com/
[esp-idf-plugin]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/vscode-setup.html



## Related Link

This detailed compilation instruction comes from this awesome repository: [Bluepad32 for Arduino](https://gitlab.com/ricardoquesada/bluepad32/-/blob/main/docs/plat_arduino.md)

- **Datasheet** 
   - [ESP32](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/esp32_datasheet_en.pdf)
   - [SHT30 Datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/unit/SHT3x_Datasheet_digital.pdf)
   - [BM8563](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/BM8563_V1.1_cn.pdf)
   - [SY7088](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/SY7088-Silergy.pdf)
   - [GT911-datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/m5paper/gt911_datasheet.pdf)

-  **API**

   - [Arduino API](https://docs.m5stack.com/en/api/m5paper/system_api)

## Schematic

   - [M5 PAPER Schematic](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/schematic/Core/m5paper/M5_PAPER_SCH.pdf)
