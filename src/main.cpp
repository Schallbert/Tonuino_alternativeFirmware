#ifndef UNIT_TEST

// Includes -------------------------
#include <Arduino.h>
#include <Defines.h>
#include <avr/sleep.h>
#include <TimerOne.h>

// LPREUSSE personal includes -------
#include <StatusLed.h>
#include <interface_KeepAlive.h>
#include <interface_UserInput.h>
#include <Folder.h>
#include <NfcTag.h>
#include <Mp3PlayerControl.h>

// ____ TONUINO LOGIC SPECIFICS_____
// Play modes
enum playMode
{
    ONELARGETRACK = 1, //So-called Hörspielmodus TODO: RE-RECORD VOICE OUTPUT
    ALBUM = 2,
    SAVEPROGRESS = 5, // TODO: RE-RECORD VOICE OUTPUT
};

// end of UserInput related functions
void waitForTrackToFinish();
void playFolder();
bool setup_folder(Folder &newFolder);
void reset_card();
void setup_card();
uint8_t voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
                  bool preview = false, int previewFromFolder = 0, int defaultValue = 0, bool exitWithLongPress = false);
void timer1_task_1ms();

// Global variables & objects -------------------------------

// Init tag reader
NfcTag nfcTagReader;
// DFPlayer Mini
Mp3PlayerControl mp3;
// Folder for queuing etc.
Folder currentFolder;
// keepAlive interface
KeepAlive aKeepAlive = KeepAlive(KEEPALIVE, false);
// userInput interface
UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
// led behavior
StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, HIGH);

// SETUP ROUTINE --------------------------------------------------------------
//-----------------------------------------------------------------------------

void setup()
{
    //Activate KeepAlive to maintain power supply to circuits
    aKeepAlive.keep_alive();

#if DEBUGSERIAL
    Serial.begin(9600); // Some debug output via serial
    Serial.println(F("Tonuino starting...."));
#endif

    aLed.set_led_behavior(StatusLed::solid);
    // Busy Pin

    // Init DFplayerMini
    // TODO: Init DFmini object here

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
    do
    {
        mp3.loop();

        //Set LED
        if (mp3.is_playing())
        {
            aLed.set_led_behavior(StatusLed::solid);
        }
        else
        {
            idleCount++; // TODO: Solve via interrupt
            aLed.set_led_behavior(StatusLed::flash_slow);
        }

        //Check shutdown timer and shutdown if necessary
        if (idleCount > MAXIDLE)
        {
            //Deactivate KeepAlive to cut power supply to circuits
            aKeepAlive.shut_down();
        }
        aUserInput->set_card_detected(nfcTagReader.is_card_present());
        userAction = aUserInput->get_user_request(); //ReadCardSerial() only returns true if a known card is present.
        switch (userAction)
        {
        case UserInput::NoAction:
            break;
        case UserInput::PlayPause:
            play_pause();
            break;
        case UserInput::NextTrack:
            next_track();
            break;
        case UserInput::PrevTrack:
            prev_track();
            break;
        case UserInput::IncVolume:
            volume_up();
            break;
        case UserInput::DecVolume:
            volume_down();
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
            break;
        }

    } while (!nfcTagReader.is_new_card_present());
    // new card present
    if (nfcTagReader.get_folder(currentFolder))
    {
        playFolder(); //Folder setup and ready to play
    }
    else
    {
        // unknown card: configure
        mp3.play_specific_file(MSG_UNKNOWNTAG);
        waitForTrackToFinish();
        setup_card();
    }
}
// END OF LOOP() ----------------------------------------------------------------------------------

uint8_t voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
                  bool preview, int previewFromFolder, int defaultValue, bool exitWithLongPress)
{
    uint8_t returnValue = defaultValue;
    uint8_t lastReturnValue = defaultValue;
    UserInput::UserRequest_e userAction = UserInput::NoAction;
    if (startMessage != 0)
    {
        mp3.play_specific_file(startMessage);
    }
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
            if (preview)
            {
                waitForTrackToFinish();
                if (!previewFromFolder)
                {
                    mp3.playFolderTrack(returnValue, 1);
                }
                else
                {
                    mp3.playFolderTrack(previewFromFolder, returnValue);
                }
            }
        }
    }
}

void reset_card()
{
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
    Serial.print(F("Karte wird neu konfiguriert!"));
#endif
    mp3.play_specific_file(MSG_UNKNOWNTAG);
    waitForTrackToFinish();
    setup_card();
}

void setup_card()
{
    mp3.pause();
#if DEBUGSERIAL
    Serial.println(F("=== setup_card()"));
#endif
    if (setup_folder(currentFolder))
    {
        // Karte ist konfiguriert -> speichern
        if (mp3.is_playing())
        {
            mp3.play_pause();
        }
        //waitForTrackToFinish();
        if (nfcTagReader.set_folder(currentFolder))
        {
            mp3.play_specific_file(MSG_TAGCONFSUCCESS); //WRITE_CARD
        }
        else
        {
            mp3.play_specific_file(MSG_ERROR); //WRITE_CARD
        }
    }
    //delay(1000); //TODO: Test whether this delay makes any sense
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
    folderId = voiceMenu(99, 301, 0, true, 0, 0, true); //TODO: Shouldn't this be 300? or 310?
    // user input: Which play mode?
    playMode = (Folder::PlayMode)voiceMenu(9, MSG_TAGLINKED, MSG_TAGLINKED, false, 0, 0, true); //TODO: Rework voice menu to fit playmodes!
    trackCount = mp3.get_trackCount_of_folder(folderId);
    // Create new folder object and copy to main's folder object
    tempFolder = Folder(folderId, playMode, trackCount); // TODO: Decide on pointer and new or other architecture!
    if (tempFolder.is_valid())
    {
        // Success! copy temporary folder to new folder.
        memcpy(&newFolder, &tempFolder, sizeof(Folder));
        return true;
    }
    return false;
}

void timer1_task_1ms()
{
    aLed.led_service();
    aUserInput->userinput_service_isr();
}

#endif //UNIT_TEST

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