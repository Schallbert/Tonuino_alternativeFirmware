### Nfc-Tag controlled Mp3 player
# "TonUINO alternative firmware"

Idea and fantastic execution found on Thorsten Voß's blog, 
re-wrote the code from scratch for better structure, readability, maintainability, and extensibility.
Does not support adcanved config features (yet) like preset lullabye timer, init volume etc.

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
