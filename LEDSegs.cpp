
//Define this library if not already defined
#ifndef _LEDSEGS_
  #define _LEDSEGS_ 26

/*
Revision History [SGD]

LO19: Add segment spacing
LO21: Add cSegOptModulateSegment option
LO22: Add low/high levels to segments (work in progress)
LO23: Bug fixes
LO24: Display Routines, code cleanup, other misc
LO25: Fixes & support for Mega - mostly a few obscure short/long issues
LO26: Added cSegActionRandom, cSegOptInvertLevel, a few minor fixes

================
Light organ library for the Sparkfun 32-LED/meter RGB LED strip with an Arduino Due/Mega
with the Bliptronics "spectrum analyzer" shield.

Steven Duff (sduff1@cox.net) November 2013
Copyright (c) 2013, Steven Duff, All Rights Reserved,
MIT license granted and included by reference.

To use this code as is, you'll need some specific components:

  1) An Arduino Due - about $49 from your favorite Arduino dealer. Recommended.
     ...Or an Arduino Mega 2560 - about $59. (More than the 32-bit Due, go figure.)
     ...Or this MAY work with other AVR boards, see below

  2) ...Stacked with a Bliptronics spectrum analyzer shield
     About $25 from sparkfun.com

  3) ...Any length of Adafruit's LPD8806 weatherproof 32-LED/meter RGB strip.
     These are $30/meter. You also need an appropriate 5V supply.
 
Using this library and the above hardware, you can very quickly get sophisticated "light organ"
effects out of your strip.

The spectrum analyzer shield provides seven, fixed-band audio amplitudes from 0..1023 each time
it's values are read out. The code here drives the LPD8806 LED strip in different-colored
"segments" of LEDs on the strip, using the spectrum samples from the shield. You define the
segments, what spectrum bands they look at, and how they behave.

Any combination of the seven fixed spectrum analyzer "bands" can be averaged into any segment.
Segments have a defined starting position, length, forground/background color and other optional
properties such as spacing, background color, etc.

By default, when you define a segment, the level of the spectrum bands determine how many LEDs in the
segment are illuminated. The higher the level, the more "on" LEDs in the segment are lit up. But
options can be defined for each segment to alter how that segment displays. You can also display
static segments or backgrounds that are not level-dependent.

A note on your Arduino board: The Due and Mega are the only boards I've tried and tested. Other AVR
boards may work, but you'll need at least about 2K SRAM and about 13K flash. Please let me know
any results with other boards. Stack usage is modest so the compiled static allocation is most of it.

One advantage to the Mega and other AVR-based boards is that the compile and upload is much faster
with the curent V1.5.x IDE than the Due.

Note that although the Due is a 3.3V part, that level is fine to drive clock and data on the LPD8806
LED strip. 5V is still required to power the strip of course.

The Sparkfun LPD8806 library is used here and needs to be imported for your sketch. You must use the
most current version, and if you're using a Due  tweak it for SPI as described below. You of course
also need an Arduino 1.5.x version IDE to use a Due.

===============
Hardware Setup:
===============

With this library, you can use either SPI output or any two digital output pins on the Due. But I
strongly recommend SPI because it's much faster. Using arbitrary digital pins there's a .2ms
per LED just for the digitalWrite() operations. For short strips this isn't a big deal, but
for longer strips the lag with the audio gets discernable. Using SPI the I/O processing delay
is negligible and you can define a lot of complex segments on long strips.

Assuming you're using SPI, note that with the Due you use the actual "SPI" header next to the SAM
chip. NOT the ICSP header pins. (thank you cjbaar).

With SPI for the Mega pull SPI from digital pin 51 for data (MOSI) and digital pin 52 for clock (SCK).
For the Due (only), attach your strip's clock lead to SPI "SCK" pin 3 (the second pin down, on the side
closest to the CPU.) Attach the data lead to MOSI pin 4 (the second pin on the other side of the header.
This attachment is a bit tricky -- the spectrum shield mounts just above this header. I wirewrapped
pigtails and brought them out to a connector.

You'll also need to solder headers to your spectrum shield -- strangely, it doesn't come with them.

Now attach a stiff 5V supply to the LED strip and join the grounds tightly between that supply and
the strip and the Arduino. I'm able to successfully drive three 5 meter strips in series with a single
5V 10A supply, though with that I avoid using full white and there is some "browndown" if everything
is illuminated. It isn't really noticeable.

I recommend at least a full 5 meter LED strip if you can afford it. Be cautious connecting these strips
as they aren't polarity or voltage protected. A careless mistake can be costly. They also are a little
more delicate than they appear even with the rubberized cover. Handle gently or you can end up with
a dead LED somewhere.

You'll need the LPD8806 library. Make sure to get the latest one from github (linked via adafruit.)
If you use a Due you have to tweak it for its faster clock. Find the conditional section:

  "#ifndef SPI_CLOCK_DIV8 ..."

(If you don't see that section anywhere you have an old version of the LPD8806 library.) Before that
#ifndef, place the following conditional code to set the Due's SPI clock divider to something within
the strip's range (here 21 = 4MHz).

  #if defined __SAM3X8E__
    #define SPI_CLOCK_DIV8 21
  #endif

Now import the modified LPD8806 library to your IDE.

===============
LEDSegs object:
===============

The LEDSegs library here is written as a C++ class. It exposes methods to make creating effects
very easy. So first, include this library or its source in your code.

  #include "LEDSegs.cpp"

Next, you need to create an "LEDSegs" object so that you can define your display segments. In the
Arduino setup() routine, instantiate an "LEDSegs" object, giving the total number of LEDs in your strip:

  strip = new LEDSegs(#LEDs in your strip);

This form of the constructor is for SPI output. If you want to use digital pins instead (remember
they're a lot slower), you give the two digital pins explicitly as short ints arguments:

  strip = new LEDSegs(#LEDs, dataPin, clockPin);
  
The constructor for the LEDSegs object initializes it to a base state with no defined segments.
You can explicitly reset the LEDSegs object at any time with a ResetStrip() call:

  strip->ResetStrip()

=========
Segments:
=========

Well, feelin' pretty good are you? Now all you have to do is define your segments and you're
ready to start jammin' to the tunes!

Define your segments also in the setup() routine. For this, simply make calls to the strip's
DefineSegment() method:

  DefineSegment(start, n, action, color, bands);

There are five arguments to DefineSegment:

  1) The starting LED (0-origin) of the segment (short int). In most cases this will be 0.

  2) The number of LEDs in this segment (short int). For a 5 meter strip this is 160.

  3) The type of basic action you want for the segment:

     cSegActionBottom: illuminates LEDs up from the first LED in the segment
     
     cSegActionTop:    illuminates LEDs down, starting from the end of the segment down
     
     cSegActionMiddle: illuminates in both directions starting from the middle of the segment
     
     cSegActionStatic: illuminates all LEDs irrespective of specrum level.
     
     cSegActionRandom: illuminates LEDs randomly in the segment's range based on level.
         The randomiztion stays fixed until you reinit the LEDSegs object, or you
         can explicity call the ResetRandom() method.
     
     cSegActionNone:   the segment is not displayed. You would only use this with custom
         display routines (below).

  4) The foreground (illumination) color (one of the RGBxxx constants defined below, or an
     LEDSegs::Color(R,G,B) value == where the RGB values are 0..127.

  5) A bitmask of cSegBand# (# from 1..7), representing which bands you want in the segment. I
     recommend you keep to the middle five (cSegBand2...cSegBand6), and avoid cSegBand1
     and cSegBand7 as they are mostly inaudible and/or noise. (Especially cSegBand7 which specs
     an almost-preposterous 16KHz center frequency.)
     
     (The spectrum shield has a farily high inherent noise level and does not have any AGC.
     Thus this code attempts to do some simple noise gating and level normalization. You can
     look at the code if you're interested.)

For example, here is a 3-segment display for a 5-meter/160-LED strip: red, green and blue, using
different frequency bands from the analyzer:

  void setup() {
    strip->new LEDSegs(160);
    strip->DefineSegment( 0,  53, cSegAction_Top,    RGBRed,   cSegBand2 | cSegBand3);
    strip->DefineSegment( 53, 53, cSegAction_Middle, RGBYellow, cSegBand4);
    strip->DefineSegment(106, 53, cSegAction_Bottom, RGBBlue,  cSegBand5 | cSegBand6);
  }

To actually show the segments in time with the audio you need to put this statment in
your loop() routine:

    strip->DisplaySpectrum(true, true);
    
And that's all you need. This will give you an LED strip action like this:

     <<....red*******|   <<..****yellow****..>>   |blue****....>>

The red segment extends down from the 1/3 point on the strip based on the level of low frequencies.
The green maps the center band and expands outward in both directions from the middle of the strip,
and the red segment goes up from the 2/3 point.

You will probably want to add some delay between display update cycles, otherwise the display
may look a little frantic. This is illustrated below.

----------------
Segment Overlap:

Segments can overlap. They are "written" to the strip in index order, so later-defined segments
with default options will overwrite lower-index segments' LEDs.

For example, if you wanted a dim white background (instead of an off level) along the entire strip
where the active segments aren't doing anything, define an initial segment for the above example:

 strip->DefineSegment( 0, 160, cSegActionStatic, RGBWhiteVeryDim, 0);
 ...the segments defined as above...
 
----------------------
Current Segment Index:

Segments are stored in an array. They are displayed in index order and referenced by index.

The "current" index is the segment assumed when you call GetSegment()/SetSegment() methods without
an explicit segment index. The first DefineSegment() call sets the current index to 0.

Each call to DefineSegment() automatically first bumps the current segment index to the next
segment, and then resets the segment's properties. The first segment you define is index 0.
Changing the current segment index does nothing to any segment until you actually call DefineSegment()
or set a segment property.

  strip->GetSegmentIndex();  //returns the current (short integer) segment index
  strip->SetSegmentIndex(n); //sets the current segment index to segment index "n".
  
You can define up to 100 segments. If you want a higher or lower max, use a #define to set
cMaxSegments before including this library.

---------------------------
Get/Set Segment Properties:

All of the properties of a segment can be read in your code and with GetSegment_property() methods.
This is mainly useful for custom display routines. These methods are overloaded and you can call
them without an index to get the value for the current segment. Or you can specify a explicit
segment index. E.g.:

  strip->GetSegment_ForeColor(); //assumes current index
  strip->GetSegment_ForeColor(isegment); //Gets forground color for specified segment isegment

Likewise the SetSegment_property([isegment, ] value) methods allow you to change properties
of a segment.

Here are the Get/Set methods for segment properties. Get methods return a type appropriate
for the item - usually a short int.

    Get/SetSegment_Action
    Get/SetSegment_BackColor
    Get/SetSegment_Bands
        SetSegment_DisplayRoutine (no Get method for this)
    Get/SetSegment_FirstLED
    Get/SetSegment_ForeColor
    Get/SetSegment_Level
    Get/SetSegment_NumLEDs
    Get/SetSegment_Options
    Get/SetSegment_Spacing

Consult the class definition below to see the full list of Get/Set methods for segments.

----------------
Segment Spacing:

Any segment can have an optional segment "spacing." A spacing of zero (the default) means all LEDs
in the segment are controlled during the segment's display.

You can set the segment's spacing to any value n >=0 with SetSegment_Spacing(n). This means that only
every (n+1)th LED in the segment's range will be addressed during a display cyle. LEDs in-between are
skipped over.

A positive spacing allows for several, spaced, overlapping segments that are "interleaved."
Spaced segments should still be defined by their full length irrespective of the spacing.

To interleave spaced segments, use the same spacing value for all, and bump the starting position
of each successive segment by +1. This works for all actions, including Middle. IMPORTANT: the
defined integer length of all the interleaved segments MUST BE IDENTICAL in order for them to
overlap and display correctly.

----------------
Segment Options:

You can define options for the segment using the SetSegment_Options(bitmask) method as follows:

  ___
  cSegNoOffOverwrite:

  This option prevents a background/off LED color from overwriting whatever is already set for
  that LED from an lower-index segment. Instead of writing the background color, it skips that LED.
  But note all LEDs are set OFF (=RGBOff) at the start of each display cycle, so this doesn't
  carry across between display cycles.
  
  This option can be used to "merge" segments over each other with the earlier segment "showing through"
  the off LEDs on the higher-index segment.

  ___
  cSegOptModulateSegment:

  This option modulates the RGB intensity of forground-color LEDs illuminated in the segment between the
  background color and the foreground color based on the spectrum level. Only illuminated (foreground)
  LEDs are affected. RGB values are scaled independently and proprotionately to the level for the segment.
  
  If the background color is RGBOff, then the intensity of the "on" LEDs will vary based on the volume of
  the bands mapped to that segment. If this option is applied to a cSegActionStatic segment, then the
  entire segment will "pulse" according to the volume of the spectrum bands.
  
  ___
  cSegOptInvertLevel:
  
  An inverted level (ie 1023 - actuallevel) is used for the display. So higher levels reduce the number
  of LEDs, rather than increasing them.

===========
Displaying:
===========

Once you have the LEDSegs object created and its segments defined, just call:

  DisplaySpectrum(doLeft, doRight)
  
in the Arduino loop() routine. This queries the current values of the spectrum analyzer, and then does a
display cycle on the LED strip with those values. doLeft and doRight are bool arguments indicating which
channels to query in the spectrum analyzer. If both are true the two channels are averaged.

You may want to put in a delay between refreshes to avoid the display refreshing too frequently. This is
especially true with SPI output where the cycles can happen 1 or 2 ms apart, giving the display an overly
active appearance. About 30 per refresh usually looks about right.

For example, here is a setup() and loop() that uses millis() to keep the time between
display cycles to a minimum of 30ms.

  void setup() {
    strip = new LEDSegs(160);
    strip->DefineSegment(0, 32, cSegActionFromBottom, RGBRed, -1);
    waitforSegmentTimeMS = 0;
  }

  void loop() {
    unsigned long startRefreshMS = millis();
    strip->DisplaySpectrum(true, true);
    while (millis() < (startRefreshMS + 30UL)) {;}
  }

_________________
Display Routines:

The basic segment definitions provide a lot of flexibility, but if you want more advanced
customizations and don't mind a little coding, you can define a "display routine" for a
segment.

To set a display routine for a segment, pass a reference to the routine you want called on each
display cycle for that segment, ie: SetSegment_DisplayRoutine(&routine-name)

A display routine gets called after the spectrum samples are gathered and have been normalized
to 0...cMaxSegmentLevel. But before the actual display processing takes place. It is a void()
procedure that is called with a single short parameter: the index of the segment.

A display routine can inspect and change almost any of the segment properties, including color,
length, starting position, current normalized spectrum level, etc. These will be the values used
during the display processing. Use the SetSegment_xxx and GetSegment_xxx methods for this. It can
also inspect and change other segments as well.

Here is an example. This was for a Christmas display. It moves a "sliding" segment up and down
the strip. The starting position of the segment is based on the total volume level, and the
slider segment's color is decided on each display cycle by which band has the largest amplitude.
There is also a background of soft blue for the whole strip.

    //**********
    const short nFirstLED = 0;      //First LED to illuminate
    const short nTotalLEDs = 160;   //160 for a 5 meter strip
    const short C7SegLen = 30;      //The length of the "slider" segment
    const short C7SegMinLevel = 80; //The min level needed to show the slider
    short C7ColorSegs[3];           //Records the index of the three 0-length color segments
    uint32_t C7SegColors[3] = {RGBRed, RGBGreen, RGBGold};  //Colors to use based on loudest band
    
    void SegmentProgramChristmas7() {
      //Define a background for the whole strip
      strip->DefineSegment(nFirstLED, nTotalLEDs, cSegActionStatic, RGBBlueVeryDim, 0);
    
      //Define the "slider" segment. Color and starting position are set in the display routine
      strip->DefineSegment(0, C7SegLen, cSegActionStatic, RGBBlue, 0x1E);
      strip->SetSegment_DisplayRoutine(&SegmentDisplayChristmas7);
    
      //Define three zero-length segments just to get the levels for the three bands we want to
      //check to set the color of the slider segment in the display routine
      C7ColorSegs[0] = strip->DefineSegment(0, 0, cSegActionNone, RGBRed, cSegBand2);
      C7ColorSegs[1] = strip->DefineSegment(0, 0, cSegActionNone, RGBGreen, cSegBand3);
      C7ColorSegs[2] = strip->DefineSegment(0, 0, cSegActionNone, RGBBlue, cSegBand4 | cSegBand5);
    }
    
    //Segment's display routine called for each display cycle
    void SegmentDisplayChristmas7(short iSegment) {
      short curLevel, startPos;
      uint32_t thiscolor;
      short colorseg, iseg, maxcolorlevel, thislevel;
      curLevel = strip->GetSegment_Level(iSegment) - C7SegMinLevel;
    
      //Find the color to use based on the "loudest" band
      maxcolorlevel = -1; colorseg = 0;
      for (iseg = 0; iseg < 3; iseg++) {
        thislevel = strip->GetSegment_Level(C7ColorSegs[iseg]);
        if (thislevel > maxcolorlevel) {maxcolorlevel = thislevel; colorseg = iseg;}
      }
    
      //Set the starting position of the strip. We have to scale the current level
      //to the possible starting positions. There's some small rounding error here
      //but it's not meaningful
      if (curLevel < 0) {strip->SetSegment_Action(iSegment, cSegActionNone);}
      else {
        strip->SetSegment_Action(iSegment, cSegActionStatic);
        strip->SetSegment_ForeColor(iSegment, C7SegColors[colorseg]);
        startPos = ((curLevel * (nTotalLEDs - C7SegLen)) /
            (cMaxSegmentLevel - C7SegMinLevel)) + nFirstLED;
        startPos = constrain(startPos, 0, nLastLED); //safety
        strip->SetSegment_FirstLED(iSegment, startPos);
      }
    }
    //***********

Another example of a display routine:

The intensity of the LPD8806 strip's LEDs isn't very linear. Even an R/G/B intensity level of 1
on this strip is bright. So a Modulate/Static segment may not show enough intensity "action" on its
own to be to your liking. If so, define a display routine that re-scales spectrum samples for the
segment. E.g.:

    void DisplayRoutineModulateHelper(short iSegment) {
      const short nCuts = 6;
      const short C2CutLevels[nCuts] = {100, 200, 350, 500, 700, 950};
      const short C2MapLevels[nCuts] = {  0,   1,  15,  50, 400, cMaxSegmentLevel};
      short iLevel, thisLevel;
      thisLevel = strip->GetSegment_Level(iSegment);
      for (iLevel = 0; iLevel < (nCuts-1); iLevel++) {
        if (thisLevel < C2CutLevels[iLevel]) {break;}
      }
      strip->SetSegment_Level(C2MapLevels[iLevel]);
    }

*/

#include "SPI.h"  
#include "LPD8806.h"

//Total spectrum analyzer shield bands and max value for a band read. Do not change this.
const short cSegNumBands=7;

//Frequency band index values bit values. Bit 0 (0x1) is the lowest freq., bit 7 (0x40) is highest.
//These are fixed by the spectrum analyzer chip used on the shield, and the number of bands cannot be changed.
//For most applications it is recommended not to use bands 1 and 7.

const short cSegBand1 = 0x01;  //63Hz center - I recommend omitting this band in most applications
const short cSegBand2 = 0x02;  //160Hz
const short cSegBand3 = 0x04;  //400Hz
const short cSegBand4 = 0x08;  //1.0KHz
const short cSegBand5 = 0x10;  //2.5KHz
const short cSegBand6 = 0x20;  //6.25KHz - Think about omitting this (6KHz is a pretty high "audible" freq.)
const short cSegBand7 = 0x40;  //16KHz - I REALLY recommend omitting this one, just noise energy.

//Software gain control constants. This provides a simple 'fast attack'/'slow decay' AGC for the input.
//InitialMax is the lowest spectrum band value to which AGC processing will apply. (AGC is applied
//separately to each of the seven spectrum bands.) As each sample is read, a fixed, assumed noise
//value is subtracted, then the max value seen for that channel is updated, but only when it is above MaxBandValue.
//Then the sampled value is scaled to the range (0..MaxSegmentLevel) using the actual max seen for that channel
//so far. The decay constant is subtracted from each channel's max on each display cycle. This sample normalization
//is applied before any custom display routine is called.

const static short cInitialMaxBandValue = 200; //Lowest max value allowed (0..1023) Normalized, after noise deduction.
const static short cMaxBandValueDecay = 2;     //Subtracted from detected max on each sample cycle
const short cMaxSegmentLevel = 1023;           //Normalized max sample value coming out of MapBandsToSegments()

//LEDSegs Segment actions. See DefineSegment and SetSegment_Action.

const short cSegActionNone = 0;        //Do nothing (undefined or do-nothing segment)
const short cSegActionFromBottom = 1;  //Fill LEDs from the first LED up, based on the current averaged band level vs the max possible
const short cSegActionFromTop = 2;     //Fill LEDs from the last down
const short cSegActionFromMiddle = 3;  //Fill LEDs from the middle out
const short cSegActionStatic = 4;      //Fill all LEDs in segment. Do not look at spectrum value.
const short cSegActionRandom = 5;      //Illuminate foreground color randomly throughout the segment

//Segment Options

const short cSegOptNoOffOverwrite = 0x01;
const short cSegOptModulateSegment = 0x02;
const short cSegOptInvertLevel = 0x04;

//Max # of segments that can be defined for a strip. Segments are "written" to the strip in index order.
//So higher-index segments can overwrite part or all of an lower-index segment.

#ifndef cMaxSegments
  #define cMaxSegments 100
#endif

//The prototype for a pointer to a segment display customizing routine that can be defined for any segme
//and will be called just before each strip refresh

typedef void (*SegmentDisplayRoutine) (short iSegment);

//Our LED strip class.

class LEDSegs {
  
  public:

    //Constructor and destructor
    LEDSegs(short nLEDs) {LEDSegsInit(nLEDs, true, 0, 0);}  //Constructor with default data/clock
    LEDSegs(short nLEDs, short pinData, short pinClock) {LEDSegsInit(nLEDs, false, pinData, pinClock);}  //Constructor with explicit data/clock
    ~LEDSegs() {delete[] objLPDStrip;}
    void LEDSegsInit(short, bool, short, short);  //Common constructor code
    
    void DisplaySpectrum(bool, bool);
    void ResetStrip();
    void ResetRandom();
    
    void SetSegmentIndex(short Idx) {segCurrentIndex = constrain(Idx, 0, cMaxSegments - 1);}
    short GetSegmentIndex() {return segCurrentIndex;}

    //The SetSegment_xxx routines are overloaded. The segment # parameter can be omitted and defaults to the current index
    
    void SetSegment_Action(short nSegment, short Action) {if (Action >= 0) {SegmentData[nSegment].segAction = Action;};}
    void SetSegment_Action(short Action) {SetSegment_Action(segCurrentIndex, Action);}
    void SetSegment_BackColor(short nSegment, uint32_t BackColor) {if (BackColor != 0xFFFFFFFF) {SegmentData[nSegment].segBackColor = BackColor;};}
    void SetSegment_BackColor(uint32_t BackColor) {SetSegment_BackColor(segCurrentIndex, BackColor);}
    void SetSegment_Bands(short nSegment, short Bands) {if (Bands >= 0) {SegmentData[nSegment].segBands = Bands;};}
    void SetSegment_Bands(short Bands) {SetSegment_Bands(segCurrentIndex, Bands);}
    void SetSegment_DisplayRoutine(short nSegment, SegmentDisplayRoutine Routine) {SegmentData[nSegment].segDisplayRoutine = *Routine;}
    void SetSegment_DisplayRoutine(SegmentDisplayRoutine Routine) {SetSegment_DisplayRoutine(segCurrentIndex, Routine);}
    void SetSegment_FirstLED(short nSegment, short FirstLED) {if (FirstLED >= 0) {SegmentData[nSegment].segFirstLED = FirstLED;};}
    void SetSegment_FirstLED(short FirstLED) {SetSegment_FirstLED(segCurrentIndex, FirstLED);}
    void SetSegment_ForeColor(short nSegment, uint32_t ForeColor) {if (ForeColor != 0xFFFFFFFF) {SegmentData[nSegment].segForeColor = ForeColor;};}
    void SetSegment_ForeColor(uint32_t ForeColor) {SetSegment_ForeColor(segCurrentIndex, ForeColor);}
    void SetSegment_Level(short nSegment, short level) {SegmentData[nSegment].segLevel = level;}
    void SetSegment_Level(short level) {SetSegment_Level(segCurrentIndex, level);}
    void SetSegment_NumLEDs(short nSegment, short nLEDs) {if (nLEDs >= 0) {SegmentData[nSegment].segNumLEDs = nLEDs;};}
    void SetSegment_NumLEDs(short nLEDs) {SetSegment_NumLEDs(segCurrentIndex, nLEDs);}
    void SetSegment_Options(short nSegment, short Options) {if (Options >= 0) {SegmentData[nSegment].segOptions = Options;};}
    void SetSegment_Options(short Options) {SetSegment_Options(segCurrentIndex, Options);}
    void SetSegment_Spacing(short nSegment, short Spacing) {if (Spacing >= 0) {SegmentData[nSegment].segSpacing = Spacing;};}
    void SetSegment_Spacing(short Spacing) {SetSegment_Spacing(segCurrentIndex, Spacing);}

    short    GetSegment_Action(short nSegment)    {return SegmentData[nSegment].segAction;}
    uint32_t GetSegment_BackColor(short nSegment) {return SegmentData[nSegment].segBackColor;}
    short    GetSegment_Bands(short nSegment)     {return SegmentData[nSegment].segBands;}
    short    GetSegment_FirstLED(short nSegment)  {return SegmentData[nSegment].segFirstLED;}
    uint32_t GetSegment_ForeColor(short nSegment) {return SegmentData[nSegment].segForeColor;}
    short    GetSegment_Level(short nSegment)     {return SegmentData[nSegment].segLevel;}
    short    GetSegment_NumLEDs(short nSegment)   {return SegmentData[nSegment].segNumLEDs;}
    short    GetSegment_Options(short nSegment)   {return SegmentData[nSegment].segOptions;}
    short    GetSegment_Spacing(short nSegment)   {return SegmentData[nSegment].segSpacing;}

    //Initialize a new segment and return the index # of the segment defined.
    //You can set spectrum bands to -1 to include all bands, or 0 to not modulate according to audio level at all
    short DefineSegment(
        short     /* First LED in segment (0 origin) */
      , short     /* # LEDs in segment*/
      , short     /* cSegActionXXX value - defines how the segment works */
      , uint32_t  /* Foreground color */
      , short     /* Bitmask of cSegBandN spectrum band specs, to be averaged together to make this segment's value */
    );
 
    //Methods that match LPD8806 member function, except declared static and does not set the high bit (this
    //is done by LPD8806 setPixelColor. Return value is GRB (not RGB!) value in long int.
    static uint32_t Color(byte r, byte g, byte b) {
      return ((uint32_t)(g) << 16) | ((uint32_t)(r) <<  8) | b;
    }
    
    //Get the r/g/b components of a color into byte values (remember value is GRB, not RGB), return array [0..2]
    //is ordered RGB
    static void Colorvals(uint32_t Color, byte rgbvals[]) {
      rgbvals[1] = ((Color >> 16) & 0x7F);
      rgbvals[0] = ((Color >> 8) & 0x7F);
      rgbvals[2] = (Color & 0x7F);
    }

  private:
    const static short cSpectrumReset=5;
    const static short cSpectrumStrobe=4;
    
    struct stripSegment {
      short segFirstLED;    //The first LED in the segment from the beginning (0-origin)
      short segNumLEDs;  //The number of LEDs in the segment
      uint32_t segForeColor;     //The base color of the segment's LEDs
      uint32_t segBackColor;     //Background color
      short segBands;       //The spectrum bands that are averaged together to make up the value for the segment
      short segAction;      //The way the LEDs in the segment are populated
      short segSpacing;     //Spacing between LEDs that are illuminated in the segment (0 default = no spacing)
      short segOptions;     //Options for the segment (cSegOpt...)
      SegmentDisplayRoutine segDisplayRoutine;  //Optional routine to call just before each display cycle
      short segLevel, segMaxLevel;       //Normalized & max level -- output from MapBandsToSegments
    };

    short segCurrentIndex;    //The "current" (default) index that will be modified
    short segMaxDefinedIndex; //Tracks the highest index defined
    stripSegment SegmentData[cMaxSegments];  //The segment array
    
    //The per-band level from the spectrum analyzer for the current sample (see ReadSpectrum)
    short SpectrumLevel[cSegNumBands];
    
    //Max value seen for each spectrum band so far. Used to implement a simple adaptive AGC.
    short maxBandValue[cSegNumBands];

    //Maximum noise values for each band. A band spectrum value of this or lower cause no illumination
    //These were determined by experimentation.
    short nNoiseFloor[cSegNumBands];
    
    //Spectrum analyzer left/right channels
    const static short cSegSpectrumAnalogLeft=0;  //Left channel
    const static short cSegSpectrumAnalogRight=1; //Right channel

    void MapBandsToSegments();
    void ReadSpectrum(bool, bool);
    void ShowSegments();
    
    //A pointer to the low-level I/O LBD8806 strip object we talk to
    LPD8806* objLPDStrip;
    short nLEDsInStrip;
    
    //Array of random cutoff levels (for cSegActionRandom)
    unsigned short segRandomLevels[64];  //Changing this requires code changes
};

//Various colors. The bit format of these is defined by the LPD8806 library.
//Assume nothing about the format except they are an unsigned long int and 0..127

const uint32_t RGBOff =         LEDSegs::Color(0, 0, 0);
const uint32_t RGBBlack =       RGBOff;

const uint32_t RGBWhite =       LEDSegs::Color(127, 127, 127); //Watch out - 60mA per LED with this
const uint32_t RGBGold =        LEDSegs::Color( 90,  40,   8);
const uint32_t RGBSilver =      LEDSegs::Color( 15,  30,  60);
const uint32_t RGBYellow =      LEDSegs::Color( 90,  70,   0);
const uint32_t RGBOrange =      LEDSegs::Color( 60,  20,   0);
const uint32_t RGBRed =         LEDSegs::Color(127,   0,   0);
const uint32_t RGBGreen =       LEDSegs::Color(  0, 127,   0);
const uint32_t RGBBlue =        LEDSegs::Color(  0,   0, 127);
const uint32_t RGBPurple =      LEDSegs::Color( 40,   0,  40);

//White-ish versions of the base colors (except white!)

const uint32_t RGBGoldWhite =   LEDSegs::Color(110,  70,  30);
const uint32_t RGBSilverWhite = LEDSegs::Color( 20,  45,  90);
const uint32_t RGBYellowWhite = LEDSegs::Color(127, 100,  15);
const uint32_t RGBOrangeWhite = LEDSegs::Color( 80,  35,   5);
const uint32_t RGBRedWhite =    LEDSegs::Color(100,   3,   5); //aka "Pink"
const uint32_t RGBGreenWhite =  LEDSegs::Color( 20, 127,  20);
const uint32_t RGBBlueWhite =   LEDSegs::Color( 10,  20, 127);
const uint32_t RGBPurpleWhite = LEDSegs::Color( 40,   8,  40);

//Dimmer versions of the base colors
const uint32_t RGBWhiteDim =       LEDSegs::Color( 8, 15, 15);
const uint32_t RGBSilverDim =      LEDSegs::Color( 8, 15, 24);
const uint32_t RGBGoldDim =        LEDSegs::Color( 8,  4,  1);
const uint32_t RGBYellowDim =      LEDSegs::Color(20, 15,  0);
const uint32_t RGBOrangeDim =      LEDSegs::Color(15,  3,  0);
const uint32_t RGBRedDim =         LEDSegs::Color(32,  0,  0);
const uint32_t RGBGreenDim =       LEDSegs::Color( 0,  6,  0);
const uint32_t RGBBlueDim =        LEDSegs::Color( 0,  0, 24);
const uint32_t RGBPurpleDim =      LEDSegs::Color(10,  0, 10);

//Very dim versions of the colors
const uint32_t RGBWhiteVeryDim =   LEDSegs::Color( 1,  2,  2);
const uint32_t RGBSilverVeryDim =  LEDSegs::Color( 1,  2,  4);
const uint32_t RGBGoldVeryDim =    LEDSegs::Color( 4,  2,  1);
const uint32_t RGBYellowVeryDim =  LEDSegs::Color( 4,  3,  0);
const uint32_t RGBOrangeVeryDim =  LEDSegs::Color( 4,  1,  0);
const uint32_t RGBRedVeryDim =     LEDSegs::Color( 1,  0,  0);
const uint32_t RGBGreenVeryDim =   LEDSegs::Color( 0,  1,  0);
const uint32_t RGBBlueVeryDim =    LEDSegs::Color( 0,  0,  1);
const uint32_t RGBPurpleVeryDim =  LEDSegs::Color( 1,  0,  1);

//General macros
#define SIZEOF_ARRAY(ary) (sizeof(ary) / sizeof(ary[ 0 ]))

/*______________
LEDSegsInit:Common constructor code
*/

void LEDSegs::LEDSegsInit(short nLEDs, bool useSPI, short pinData, short pinClock) {
  unsigned short iBand;
  
  segCurrentIndex = 0;
  segMaxDefinedIndex = -1;

  //Noise values for each spectrum band (0..1023). Determined by experimentation. YMMV
  nNoiseFloor[0] =  90;
  nNoiseFloor[1] =  90;
  nNoiseFloor[2] =  90;
  nNoiseFloor[3] = 100;
  nNoiseFloor[4] = 100;
  nNoiseFloor[5] = 110;
  nNoiseFloor[6] = 120;
    
  //Initialize the max level seen for each band.
  for (iBand = 0; iBand < cSegNumBands; iBand++) {maxBandValue[iBand] = cInitialMaxBandValue;}

  //Create an LED strip object. Either SPI or digital pins

  if (useSPI) {objLPDStrip = new LPD8806(nLEDs);}
  else {objLPDStrip = new LPD8806(nLEDs, pinData, pinClock);}
  
  
  nLEDsInStrip = nLEDs;
  
  //Setup pins to drive the spectrum analyzer. 
  pinMode(cSpectrumReset, OUTPUT);
  pinMode(cSpectrumStrobe, OUTPUT);

  //Init spectrum analyzer to start reading from lowest band
  digitalWrite(cSpectrumStrobe,LOW);
    delay(1);
  digitalWrite(cSpectrumReset,HIGH);
    delay(1);
  digitalWrite(cSpectrumStrobe,HIGH);
    delay(1);
  digitalWrite(cSpectrumStrobe,LOW);
    delay(1);
  digitalWrite(cSpectrumReset,LOW);
    delay(5);

  //Init this guy
  ResetStrip();
}

/*____________________
LEDSegs::DefineSegment
Set the properties of the current LED segment. A -1 value indicates that the corresponding property should not be changed.
Return value is the segment index.
*/

short LEDSegs::DefineSegment(short FirstLED, short nLEDs, short Action, uint32_t ForeColor, short Bands) {

  //Move to next segment (if no segments yet, start with #0)
  if (segMaxDefinedIndex < 0) {SetSegmentIndex(0);} else {SetSegmentIndex(segCurrentIndex + 1);}
  
  //Set the segment properties passed in
  SetSegment_FirstLED(FirstLED);
  SetSegment_NumLEDs(nLEDs);
  SetSegment_Action(Action);
  SetSegment_ForeColor(ForeColor);
  SetSegment_Bands(segCurrentIndex, Bands);
  
  //Segment defaults
  SetSegment_BackColor(RGBOff);
  SetSegment_Spacing(0);
  SetSegment_Options(segCurrentIndex, 0);
  SetSegment_DisplayRoutine(segCurrentIndex, NULL);

  //Track the highest segment index defined. This speeds the refresh loop a bit.
  segMaxDefinedIndex = max(segMaxDefinedIndex, segCurrentIndex);

  //Return the segment index that was updated, before incrementing it
  return segCurrentIndex;
};

/*______________________
LEDSegs::DisplaySpectrum
Sample and display according to the defined segments
*/

void LEDSegs::DisplaySpectrum(bool doLeft, bool doRight) { 
  ReadSpectrum(doLeft, doRight);
  MapBandsToSegments();
  ShowSegments();
};

/*_________________________
LEDSegs::MapBandsToSegments
Convert spectrum band samples into LED strip segment values in the range 0 .. (# LEDs in that segment).
We average all the bands defined for the segment, and then scale the final segment value
*/

void LEDSegs::MapBandsToSegments() {
  short iSegment, iBand, nLEDs, segBands;
  unsigned long maxTotal, sampleTotal;
  SegmentDisplayRoutine thisDisplayRoutine;
  
  //Loop all defined segments to calc the normalized band value. We do this even for ActionNone segments
  //in case a segment display routine wants to change the action
  for (iSegment = 0; iSegment <= segMaxDefinedIndex; iSegment++) {
    nLEDs = SegmentData[iSegment].segNumLEDs;
    segBands = SegmentData[iSegment].segBands;

    //Loop spectrum bands. For any that are mapped into this segment we total both the sample values and the
    //max possible values, in order to do the normalization.
    maxTotal = 0;
    sampleTotal = 0;

    for (iBand = 0; iBand < cSegNumBands; iBand++) {
      if ((segBands >> iBand) & 1) {
        maxTotal += maxBandValue[iBand];
        sampleTotal += SpectrumLevel[iBand];
      }
    }
    if (maxTotal <= 0) {maxTotal = 1;} //Safety for use as divisor
      
    //Normalize the averaged level to 0..1023 and record in the level array element for this segment
    sampleTotal = (sampleTotal * cMaxSegmentLevel) / maxTotal;
    SegmentData[iSegment].segLevel = sampleTotal;
    SegmentData[iSegment].segMaxLevel = maxTotal;
  } //end segments loop
  
  //Now that all the segments are setup, call any segment display routines that are defined
  for (iSegment = 0; iSegment <= segMaxDefinedIndex; iSegment++) {
    thisDisplayRoutine = SegmentData[iSegment].segDisplayRoutine;
    if (thisDisplayRoutine != NULL) {thisDisplayRoutine(iSegment);}
  };  
};  

/*___________________
LEDSegs::ReadSpectrum
Read the spectrum band samples into class array SpectrumLevel[].
"Channels" tells whether to read left, right, or average both channels.
*/
void LEDSegs::ReadSpectrum(bool doLeft, bool doRight) {
  short iBand, thisLevel, bandMax;  //Band 0 is lowest frequencies, Band 6 is the highest.

  //This loop happens nBands times per sample, so keep it quick. It just records the
  //current and max sample values into the band value arrays.
  for(iBand=0; iBand < cSegNumBands; iBand++) {
    
    //Decay the max a little on each sample
    bandMax = maxBandValue[iBand] - cMaxBandValueDecay;
    if (bandMax < cInitialMaxBandValue) bandMax = cInitialMaxBandValue;
    
    //Read the spectrum for this band
    thisLevel = 0;
    if (doLeft) {thisLevel += analogRead(cSegSpectrumAnalogLeft);}
    if (doRight) {thisLevel += analogRead(cSegSpectrumAnalogRight);}
    if (doLeft && doRight) {thisLevel = thisLevel >> 1;} //If both channels, then take average

    //Process out assumed noise floor for this band
    thisLevel -= nNoiseFloor[iBand];
    if (thisLevel < 0) {thisLevel = 0;}

    //Set current and max values for this into their respective segment object array slots
    SpectrumLevel[iBand] = thisLevel;
    if (bandMax < thisLevel) {bandMax = thisLevel;}
    maxBandValue[iBand] = bandMax;
    
    //Toggle to ready for next band
    digitalWrite(cSpectrumStrobe,HIGH);
    digitalWrite(cSpectrumStrobe,LOW);     
  }
}

/*_________________
LEDSegs::ResetStrip
Reset the whole thing
*/

void LEDSegs::ResetStrip() {
  short i;
  
  //Reset segment array
  for (i = 0; i < cMaxSegments; i++) {SegmentData[i].segAction = cSegActionNone;}
  
  objLPDStrip->begin();  //Clear and init the strip
  objLPDStrip->show();  //Update the LED strip display to show off to start
  segCurrentIndex = 0;
  segMaxDefinedIndex = -1;

  //Init the random permutation array (for cSegActionRandom)
  ResetRandom();
}

/*__________________
LEDSegs::ResetRandom
Init the cSegActionRandom levels array
*/

void LEDSegs::ResetRandom() {
  unsigned short i, imax;

  randomSeed(micros());
  imax = SIZEOF_ARRAY(segRandomLevels);
  
  //Init the random permutation array (Knuth shuffle)
  for (i=0; i < imax; i++) {segRandomLevels[i] = random(cMaxSegmentLevel);}
}

/*___________________
LEDSegs::ShowSegments
Display the segment values on the LED strip
*/

void LEDSegs::ShowSegments() {
  short    iSegment, iLEDinSegment, iLED, LEDIncrement, segval, ledval;
  short    FirstLED, NumberLEDs, Action, Options, segSpacing1, SpacingCount;
  bool     optOffOverwrite, optModulate, notSpacingLED, doled;
  uint32_t thisColor, backColor, foreColor;
  byte     bcRGB[3], fcRGB[3]; //extra byte for long align
  stripSegment *segptr;

  //First, init all LEDs in the strip to off
  for (iLED = 0; iLED < nLEDsInStrip; iLED++) {objLPDStrip->setPixelColor(iLED, RGBOff);}
  
  //Write each defined segment
  for (iSegment = 0; iSegment <= segMaxDefinedIndex; iSegment++) {
      
    segptr = &SegmentData[iSegment];
    Action = segptr->segAction;

    //Process segment if it does something

    if (Action != cSegActionNone) {
 
      /* Set some local vars for fast reference that we'll need */
      FirstLED = segptr->segFirstLED;
      NumberLEDs = segptr->segNumLEDs;
      backColor = segptr->segBackColor;
      foreColor = segptr->segForeColor;
      segSpacing1 = segptr->segSpacing + 1;

      Options = segptr->segOptions;
      optOffOverwrite = (Options & cSegOptNoOffOverwrite) == 0;
      optModulate = (Options & cSegOptModulateSegment) != 0;
      
      //The level coming out of MapBandsToSegments() is normalized to 0..1023. Here we
      //scale to the number of LEDs that means for this segment.
      
      if (Options & cSegOptInvertLevel) {segptr->segLevel = cMaxSegmentLevel - segptr->segLevel;}
      segval = segptr->segLevel;
      segval = (((long) segval) * ((long) (NumberLEDs + 1))) / ((long) (cMaxSegmentLevel + 1));
      segval = constrain(segval, 0, NumberLEDs); //Insure within expected range
      ledval = segval;
      if ((Action == cSegActionStatic) || (Action == cSegActionRandom)) {ledval = NumberLEDs;}

      //If this is a ModulateSegment option segment, then figure the foreground color scaled between
      //backcolor and forecolor according to the segment's spectrum level.
      if (optModulate) {
        Colorvals(backColor, bcRGB);
        Colorvals(foreColor, fcRGB);
        foreColor = LEDSegs::Color(
            bcRGB[0] + (((fcRGB[0] - bcRGB[0]) * segval) / NumberLEDs)
          , bcRGB[1] + (((fcRGB[1] - bcRGB[1]) * segval) / NumberLEDs)
          , bcRGB[2] + (((fcRGB[2] - bcRGB[2]) * segval) / NumberLEDs));
      }
  
      //Get the starting LED index for this segment and an initial increment to get to the next LED
      switch (Action) {
        case cSegActionFromBottom: //bottom, static and random start iLED the same
        case cSegActionRandom:
        case cSegActionStatic:     LEDIncrement = 1;  iLED = FirstLED; break;

        case cSegActionFromTop:    LEDIncrement = -1; iLED = FirstLED + NumberLEDs - 1; break;

        case cSegActionFromMiddle: LEDIncrement = 0;  iLED = FirstLED + ((NumberLEDs - 1) >> 1); break;
      }

      //Init the spacing counter. This counts down from spacing-1 each time it hits 0 (an illuminated LED).
      //The first LED is always a non-spacer, which is why we init to zero.

      SpacingCount = 0;
      
      //Loop across the LEDs in the segment being illuminated.
      //NOTE: Keep this loop TIGHT! It is run for every LED in every defined segment

      for (iLEDinSegment = 0; iLEDinSegment < NumberLEDs; iLEDinSegment++) {

        //Figure the color we want this LED to be and write it to the LED
        thisColor = backColor;
        if (ledval > iLEDinSegment) {thisColor = foreColor;}

        //Determine if this LED is blanked out because of spacing > 1
        notSpacingLED = (SpacingCount == 0);

        //Write the color value to the strip, but not if...
        //  1) This is a spacing LED (when segment's spacing value is > 1), or...
        //  2) The color value is RGBOff and this is a no-off-overwrite-option segment
        //  3) This is an ActionRandom segment and the level is too low based on the randomizer
        
        doled = (notSpacingLED & ((thisColor != RGBOff) | optOffOverwrite));
        if (doled & (Action == cSegActionRandom)) {
          if (segRandomLevels[iLEDinSegment & 0x3F] > segptr->segLevel) {doled = false;}
        }

        if (doled) {objLPDStrip->setPixelColor(iLED, thisColor);}
   
        //Move to next LED. For from-middle, we jump back and forth around the center of the segment, increasing
        //the increment's absolute value by one more each jump.

        if (Action == cSegActionFromMiddle) {
          if (LEDIncrement <= 0) {
            LEDIncrement--;
            if (notSpacingLED) {SpacingCount = segSpacing1;}
            SpacingCount--;
          }
          else {LEDIncrement++;}
          LEDIncrement = -LEDIncrement;
        }
        else {
          if (notSpacingLED) {SpacingCount = segSpacing1;}
          SpacingCount--;
        }

        iLED += LEDIncrement;
      } //LED-in-segment loop
    } //If an action defined
  }  //Segment loop

  //Finally, refresh the strip.
  objLPDStrip->show();
}
#endif  //_LEDSEGS_

