### Nfc-Tag controlled Mp3 player
# "TonUINO alternative firmware"

Idea and fantastic execution found on Thorsten Voß's blog, 
re-wrote the code from scratch for better structure, readability, maintainability, and extensibility.
Does not support advanced config features (yet) like preset lullabye timer, init volume etc.

## Features:
* Configurable user input (buttons or encoder)
* Auto-Poweroff when no button pressed for a configurable time
* Power on via "play" button press
* Autoplay feature
* Status Led feature
* User Input lock/unlock feature
* Multiple playmodes [Album, Random, SaveProgress, OneTrack] available per Nfc Tag
* Voice menus for linking or deletion of Nfc Tags
* Optimized for battery applications (e.g. powerbank) using sleep states
* Low power consumption @5V: ~40mA in idle, ~75mA playing medium volume
* Config file for init volume, lullabye timer etc.
* Auto-recovery from stuck prompts
* Voice prompts for most common state errors

## So much work...
My first real OO C++ project, and, maybe, it was a little big to start with.
All these concepts of clean code, writing to an interface, first-time usage of Platformio as IDE,
utilizing the googletest unit test framework, using coding patterns like factory or Dependency Injection took nearly a year of evenings before this project could be completed. And there's always something left to refactor and to improve :P
### Quick facts
* 400+ hours of total development time...
* Unit test suite of 250+ test cases
* Serial debug output configurable
* Loosely coupled C++ OO architecture
* Individual modules with clear tasks, scaleable and easily accessible for future feature add
* custom Hardware abstraction layer; most of the code should be portable to other mcus without changes
* custom Dependency Injection framework (Loader class)

## Additional notes:
RAM usage of >75% will likely cause segfaults (been through that!)
When battery powered, I'd propose to use a bi-stable relay so the Tonuino 2.0 can keep alive without using additional power for coils and - when finished - cut its own power supply.
Only tested 1k Mifare Nfc tags, 4k and Mini will also work, Ultralight SHOULD work (but not integration tested).

# Documentation
The Project is written almost exclusively in C++ and build for Arduino Framework - tested on an Arduino nano board - using the [PlatformIO IDE](https://platformio.org/).
The following sections show the structure and the libraries needed (which will be pulled in automatically once trying to build by the PlatformIO).
## Project Module overview
Folder name in `/lib` | Purpose
--- | ---
Arduino	| minimalistic close-to-Hardware implementations, not unit-testable
Arduino_HardwareAbstraction	| Hardware abstraction to enable portability and testing
Config | System configuration parameters
Folder | Playlist and playmode business logic
Loader | Dependency Injection Framework
MessageHandler | System messages and Debug Framework
Mp3	Mp3 control | (Status, Folder, Prompts, Advertisements)
Nfc	Tag control | (Status, Read, Write, Delete)
PowerManager | Manager for Status Led and Keep Alive 
Tonuino | Main task scheduler
UserInput | Button or Encoder input handling
Utilities | Timers, Led Control, Pin control
VoiceMenu | Link / Delete / Config menu business logic

## External Libraries
![platformio.ini lib section](https://github.com/Schallbert/Tonuino_alternativeFirmware/edit/master/docs/LibrariesUsed.png "platformio.ini")

## Class Diagrams
Yeah, this is still TODO :/

## Unit tests
Are configured an can be built and run using `pio test -e desktop -f desktop` in the PlatformIO CLI terminal. Note that *googletest* will require `gcc` with some libraries to be installed, how to can be found [here](https://community.platformio.org/t/unit-testing-with-gtest-gmock-on-env-desktop-on-arduino-platform/14354). 

# Hardware
## Material
TODO
## Schematics
TODO
