# Wordclock
THIS IS NOT A MANUAL, YOU HAVE TO FIGURE OUT HOW TO BUILD IT YOURSELF

Added pictures for some clarity

These are the sources for my Wordclock project. 
Lots of things are in Dutch since I never planned to go public with it, so you have to do some figuring out.
On the other hand, I figured out a lot that you now can use.
Also, be aware that still a lot of improvements can be made on both software and components.

I used a lot of stuff that's on the internet to start out with, so most of it is not originally mine.

The faceplate
I created the face plate in Inkscape and adjusted the DroidSansMono.ttf to meet my needs in FontForge
https://inkscape.org/release/inkscape-1.2.1/windows/64-bit/msi/?redirected=1
https://fontforge.org/en-U

For hardware I used:
5 meters WS2812b digital RGB ledstrip Premium Model: DS12BLS60-05M0520
40 Watt - 5V / 8A adapter for USB led strips Model: AD05-08PT
Between the Faceplate and the Backplate I used a sample of ORACAL® 8500 Translucent Color: 8500-010 White
The Backplate is MDF black 500x500 mm, 22 mm thick
Arduino Mega2560Pro
nodemcu esp32-s 38 pins (many types available)
DS3231 clock
BME260 sensor for fun
Diode and resisters where needed

Designed PCB's to hold the LEDs and the Arduino and Clock etc, designed them in Fritzing, ordered it at https://aisler.net/

Tip: Don't solder the Leds on the backstrips any hotter than 290 degrees Celcius or they'll get damaged and you'll spend a lot of time replacing bad ones.
Talking from experience here :-(

