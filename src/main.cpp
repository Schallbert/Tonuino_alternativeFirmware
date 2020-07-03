#ifndef UNIT_TEST

// Includes -------------------------
#include <Arduino.h>
#include <Defines.h>
#include <avr/sleep.h>
#include "TimerOne/src/TimerOne.h"

// LPREUSSE personal includes -------
#include <StatusLed.h>
#include "interface_KeepAlive/interface_KeepAlive.h"
#include <interface_UserInput.h>
#include <Folder.h>
#include <NfcTag.h>
#include <Mp3PlayerControl.h>
#include <EEPROM_implementation.h>
#include <MFRC522_implementation.h>

// Function prototypes
uint32_t init_random_generator(); // External dependency: Randum Number Generator
bool setup_folder(Folder &newFolder);
void reset_card();
void setup_card();
uint8_t voice_menu(uint8_t numberOfOptions, uint16_t startMessage, bool returnValuesOffsetStartMessage,
                   bool previewSelectedFolder = false, uint8_t defaultValue = 0);
void timer1_task_1ms();

// Global variables & objects -------------------------------
// Init tag reader
Mfrc522 tagReader;
NfcTag nfcTagReader = NfcTag(&tagReader); // Constructor injection of concrete reader
// DFPlayer Mini
Mp3PlayerControl mp3;
// Folder for queuing etc.
Folder currentFolder;
// keepAlive interface
KeepAlive aKeepAlive = KeepAlive(KEEPALIVE, false, MAXIDLE);
// userInput interface
UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
// led behavior
StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, HIGH);
// Eeprom init
Eeprom eeprom;
//

// SETUP ROUTINE --------------------------------------------------------------
//-----------------------------------------------------------------------------
void setup()
{
    aKeepAlive.keep_alive(); //Activate KeepAlive to maintain power supply to circuits

#if DEBUGSERIAL
    Serial.begin(9600); // Some debug output via serial
    Serial.println(F("Tonuino starting...."));
#endif

    aLed.set_led_behavior(StatusLed::solid);

    //Init Timer1 for Encoder read
    //init UserInput
    aUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    aUserInput->init();

#if DEBUGSERIAL
    Serial.println(F("Tonuino started."));
#endif
}
// LOOP ROUTINE --------------------------------------------------------------
//----------------------------------------------------------------------------
void loop()
{
    UserInput::UserRequest_e userAction = UserInput::NoAction;
    while (!nfcTagReader.is_new_card_present())
    {
        // No new card present. Accept user input.
        mp3.loop(); // Autoplay and DFmini loop
        //Set LED
        if (mp3.is_playing())
        {
            aKeepAlive.set_idle_timer(false);
            aLed.set_led_behavior(StatusLed::solid);
        }
        else
        {
            aKeepAlive.set_idle_timer(true);
            aLed.set_led_behavior(StatusLed::flash_slow);
        }
        aUserInput->set_card_detected(nfcTagReader.is_card_present());
        userAction = aUserInput->get_user_request(); //ReadCardSerial() only returns true if a known card is present.
        switch (userAction)
        {
        case UserInput::NoAction:
            break;
        case UserInput::PlayPause:
            mp3.play_pause();
            break;
        case UserInput::NextTrack:
            mp3.next_track();
            break;
        case UserInput::PrevTrack:
            mp3.prev_track();
            break;
        case UserInput::IncVolume:
            mp3.volume_up();
            break;
        case UserInput::DecVolume:
            mp3.volume_down();
            break;
        case UserInput::DelCard:
            reset_card();
            break;
        case UserInput::Help:
            mp3.play_specific_file(MSG_HELP);
            break;
        case UserInput::Abort:
            break;
        case UserInput::Error:
#if DEBUGSERIAL
            Serial.println(F("Error getting UserInput. Not fully initialized/pins set?"));
#endif
            mp3.play_specific_file(MSG_ERROR);
            break;
        }
    }
    // Do not allow automatic shutdown
    aKeepAlive.set_idle_timer(false);
    aLed.set_led_behavior(StatusLed::dim);
    // new card present, load information from card, create and return folder
    if (nfcTagReader.read_folder_from_card(currentFolder))
    {
        currentFolder.setup_dependencies(&eeprom, init_random_generator());
        mp3.play_folder(&currentFolder); //Folder setup and ready to play
    }
    else
    {
        // unknown card: configure
        mp3.play_specific_file(MSG_UNKNOWNTAG);
        mp3.dont_skip_current_track();
        setup_card();
    }
}
// END OF LOOP() ----------------------------------------------------------------------------------

uint8_t voice_menu(uint8_t numberOfOptions, uint16_t startMessage, bool returnValuesOffsetStartMessage,
                   bool previewSelectedFolder, uint8_t defaultValue)
{
    uint8_t returnValue = defaultValue;
    uint8_t lastReturnValue = defaultValue;
    uint16_t messageOffset = (uint8_t)returnValuesOffsetStartMessage * startMessage;
    UserInput::UserRequest_e userAction = UserInput::NoAction;
    if (startMessage == 0)
    {
        return defaultValue; // invalid function call
    }
    if (mp3.is_playing())
    {
        mp3.play_pause();
    }
    mp3.play_specific_file(startMessage);
#if DEBUGSERIAL
    Serial.print(F("=== voiceMenu() ("));
    Serial.print(numberOfOptions);
    Serial.println(F(" Options)"));
#endif
    while (true)
    {
        lastReturnValue = returnValue;
#if DEBUGSERIAL
        //get command from Serial
        if (Serial.available() > 0)
        {
            int optionSerial = Serial.parseInt();
            if (optionSerial != 0 && optionSerial <= numberOfOptions)
                return optionSerial;
        }
#endif
        aUserInput->set_card_detected(true);
        userAction = aUserInput->get_user_request(); //input acts as if a card was present: needed to detect Abort signal
        mp3.loop();
        switch (userAction)
        {
        case UserInput::NoAction:
            break;
        case UserInput::Abort:
            mp3.play_specific_file(MSG_ABORTEED);
            return defaultValue;
        case UserInput::PlayPause:
            if (returnValue != 0)
            {
#if DEBUGSERIAL
                Serial.print(F("=== "));
                Serial.print(returnValue);
                Serial.println(F(" ==="));
#endif
                return returnValue;
            }
            break;
        case UserInput::NextTrack:
            returnValue = min(returnValue + 1, numberOfOptions);
            break;
        case UserInput::PrevTrack:
            returnValue = max(returnValue - 1, 1);
            break;
        default:
            break;
        }

        if (lastReturnValue != returnValue)
        {
#if DEBUGSERIAL
            Serial.println(returnValue);
#endif
            // play number of current choice, e.g. "one".
            mp3.play_specific_file(messageOffset + returnValue);
            if (previewSelectedFolder)
            {
                mp3.dont_skip_current_track();
                // Preview: Play first track from folder to link
                Folder previewFolder = Folder(returnValue, Folder::ONELARGETRACK, 1);
                previewFolder.setup_dependencies(&eeprom, 0);
                mp3.play_folder(&previewFolder);
            }
        }
    }
}

void reset_card()
{
    aLed.set_led_behavior(StatusLed::flash_quick);
    UserInput::UserRequest_e userAction = UserInput::NoAction;
    mp3.play_specific_file(MSG_DELETETAG);
    while (!nfcTagReader.is_new_card_present())
    {
        // Wait for new card to be detected, meanwhile allow to abort
        aUserInput->set_card_detected(nfcTagReader.is_new_card_present());
        userAction = aUserInput->get_user_request();
        if (userAction == UserInput::Abort)
        {
#if DEBUGSERIAL
            Serial.print(F("Aborted!"));
#endif
            mp3.play_specific_file(MSG_ABORTEED);
            return;
        }
    }
#if DEBUGSERIAL
    Serial.print(F("Card to be configured!"));
#endif
    mp3.play_specific_file(MSG_UNKNOWNTAG);
    mp3.dont_skip_current_track();
    setup_card();
}

void setup_card()
{
#if DEBUGSERIAL
    Serial.println(F("=== setup_card()"));
#endif
    if (setup_folder(currentFolder))
    {
        // Configuring card was successful.
        if (mp3.is_playing())
        {
            mp3.play_pause();
        }
        if (nfcTagReader.write_folder_to_card(currentFolder))
        {
            mp3.play_specific_file(MSG_TAGCONFSUCCESS); //WRITE_CARD
        }
        else
        {
            mp3.play_specific_file(MSG_ERROR); //WRITE_CARD
        }
    }
    delay(WAIT_DFMINI_READY);
}

/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
setup_folder() asks the user to select a folder on the SD card and to specify a playmode
 */
bool setup_folder(Folder &newFolder)
{
    // Get folder number on SD card
    uint8_t folderId = 0;
    uint8_t trackCount = 0;
    Folder::PlayMode playMode = Folder::UNDEFINED;
    Folder tempFolder;
    // user input: Which folder to link?
    folderId = voice_menu(99, MSG_UNKNOWNTAG, false, true, 0); //TODO: Shouldn't this be 300? or 310?
    // user input: Which play mode?
    playMode = (Folder::PlayMode)voice_menu((uint8_t)Folder::PlayMode::ENUM_COUNT, MSG_TAGLINKED, true, false, (uint8_t)Folder::PlayMode::ALBUM);
    trackCount = mp3.get_trackCount_of_folder(folderId);
    // Create new folder object and copy to main's folder object
    tempFolder = Folder(folderId, playMode, trackCount);
    tempFolder.setup_dependencies(&eeprom, init_random_generator());
    if (tempFolder.is_valid())
    {
        // Success! copy temporary folder to new folder.
        newFolder = tempFolder;
        return true;
    }
    return false;
}

uint32_t init_random_generator()
{
    uint32_t ADC_LSB;
    uint32_t ADCSeed;
    for (uint8_t i = 0; i < 128; i++)
    {
        ADC_LSB = analogRead(PINANALOG_RNDMGEN) & 0x1;
        ADCSeed ^= ADC_LSB << (i % 32);
    }
    return ADCSeed; // Init Arduino dependencies generator
}


void timer1_task_1ms()
{
    aLed.led_service();
    aUserInput->userinput_service_isr();
    aKeepAlive.idle_timer_tick1ms();
    mp3.lullabye_timeout_tick1ms();
}

//Helper routine to dump a byte array as hex values to Serial.
#if DEBUGSERIAL
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
#endif //DEBUGSERIAL
#endif //UNIT_TEST
