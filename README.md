LightOrgan
==========

This is Arduino code to create sound-based color effects on the Adafruit LPD8806 LED
strips, using the Ardunio Due or Mega boards with the spectrum analyzer shield.

The code is written as a C++ class. It is designed to make it easy to implement
even quite complex display schemes by just calling the various class methods.

This should work with other AVR boards provided they have 2K sram and 16K flash, but has not been tested
on any except the Mega and Due.

LEDSegs.cpp should be included and the setup() and loop() routines implemented as
described in the preamble comments in LEDSegs.cpp. An example to implement a Christmas
display is included.
