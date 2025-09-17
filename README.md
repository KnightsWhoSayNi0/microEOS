# *microEOS*

<img align="right" width="100px" src="/img/microEOS-white.png">

Teensy based USB interpreter for the ETC microvisionFX lighting console. Send 
OSC packets over USB to ETC EOS.

___

## Usage
First, we'll need to setup our hardware. Open up your *microvisionFX* console 
and gut it. We will only need the faceplace board. All other boards and cables 
can be removed (safely!).

> ⚠️ Warning! \
> Be careful of the power supply! Mains voltages are exposed and capacitors 
could contain stored energy.

We'll be connecting the pins on our teensy to the pins of the **J3** header on 
the face board. You can use a bucket connector or directly attach with dupont 
connectors. Note down the order you connect your pins.
<!-- todo: make pin table -->

Make sure to connect the VCC and GND from the teensy to the microvision board.

Next, we'll flash the code to the teensy. Clone the repository if you haven't 
already:

```
git clone https://github.com/KnightsWhoSayNi0/microEOS.git
```

You'll need to install the 
[teensy loader tool](https://www.pjrc.com/teensy/loader.html) to flash the 
code.

Open the `src/mvfx_osc/mvfx_osc.ini` in Arduino IDE (or compile on your own) 
and flash using the teensy loader.

> If necessary, change the pin numbers in `definitions.h`

## Development

TODO:
- Implement LED subscribtions
- Figure out what to do with effects section
- Cram mini pc inside microvisionFX chassis

