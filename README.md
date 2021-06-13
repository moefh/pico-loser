# pico-loser

This is an old game, Loser Corps, ported to the Raspberry Pi Pico board using custom code based on bitluni's ESP32Lib to output the game screen to a VGA monitor. It supports Wii nunchuk and classic controllers via I2C. There's currently no sound output.

![Game Screen](images/screen.jpg)

## Compilation

The `game` directory contains a CMake project that uses the official [pico-sdk](https://github.com/raspberrypi/pico-sdk). To build using the command line, install and set up the the Pico SDK, then:

    git clone https://github.com/moefh/pico-loser.git
    cd pico-loser/game
    mkdir build
    cd build
    cmake ..
    make

This will create the file `game/pico-loser.uf2` inside the `build` directory. To run the game, start the Pico in BOOTSEL mode (plug the USB cable while holding the BOOTSEL button) and copy the file `pico-loser.uf2` to the Pico folder that appears.

## VGA Output

The VGA output has 2 bits per color channel (which results in an image of 64 colors). I use some very simple homemade DACs built with a few resistors on a perfboard:

![VGA Board](images/pico.jpg)

The output pins (which can be changed in `game/main.c`) are configures as follows:

|  Pico GPIO |  Output signal  |
|-----------:|-----------------|
|          2 | Red low         |
|          3 | Red high        |
|          4 | Green low       |
|          5 | Green high      |
|          6 | Blue low        |
|          7 | Blue high       |
|          8 | Horizontal sync |
|          9 | Vertical sync   |

A resistor ladder or some other form of DAC is needed to convert the digital color signals to analog voltages expected by the VGA monitor. I use this simple setup with 3 resistors per color:

![Color DAC](images/dac-schematic.png)

## Controller Support

The game supports the Wii Nunchuk and the Wii Classic Controller using I2C. The controller pins should be connected as follows:

| Pico Pin         | Controller   |
|-----------------:|--------------|
| 3V3 OUT (pin 36) | 3V3          |
| Any GND pin      | GND          |
| GPIO 12          | SDA (data)   |
| GPIO 13          | SCL (clock)  |

You can use breakout board [like this one from Adafruit](https://www.adafruit.com/product/4836), or (if you're OK with destroying you controller's plug) follow [this tutorial](https://create.arduino.cc/projecthub/infusion/using-a-wii-nunchuk-with-arduino-597254).
