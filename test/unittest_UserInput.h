#include <Arduino.h>
#include <unity.h> //for unittest
#include <Defines.h>
#include <interface_UserInput.h>

namespace userinput
{
    void userInput_3Buttons_pinsNotSet(void);
    void userInput_3Buttons_notInitialized(void);
    void userInput_3Buttons_ready(void);
    void userInput_3Buttons_noAction(void);
    void userInput_3Buttons_playPause(void);

    void userInput_encoder_pinsNotSet(void);
    void userInput_encoder_notInitialized(void);
    void userInput_encoder_ready(void);


    void unittests(void);
} // namespace userinput
