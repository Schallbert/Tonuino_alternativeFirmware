#include "System.h"

InputManager::eCardState InputManager::getCardState_fromReader()
{
    bool cardPresent = m_nfcTagReader.is_card_present();
    m_userInput.set_card_detected(cardPresent); // TODO: needed?

    if (m_bWriteCardError)
    {
        m_bWriteCardError = false;
        return ERROR_CARD_NOWRITE;
    }

    if (cardPresent)
    {
        if (!m_nfcTagReader.is_new_card_present())
        {
            return ACTIVE_KNOWN_CARD;
        }
        else // New card detected: runs once as new card is automatically set to ActiveCard
        {
            if (m_nfcTagReader.read_folder_from_card(m_currentFolder))
            {
                return NEW_KNOWN_CARD;
            }
            else // New card but folder cannot be read
            {
                if (!m_nfcTagReader.is_known_card())
                {
                    return UNKNOWN_CARD_MENU;
                }
                else
                {
                    return ERROR_CARD_NOREAD;
                }
            }
        }
    }
    else // No card present
    {
        return NO_CARD;
    }
}

Folder InputManager::readFolderFromCard()
{
    return m_currentFolder;
}

void InputManager::writeFolderToCard(Folder target)
{
    if (!m_nfcTagReader.write_folder_to_card(target))
    {
        m_bWriteCardError = true;
    }
    else
    {
        m_currentFolder = target;
    }
}

void InputManager::deleteCard()
{
    m_nfcTagReader.erase_card();
}

UserInput::UserRequest_e InputManager::getUserInput()
{
    return m_userInput.get_user_request();
}

void OutputManager::setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput)
{
    // set to input values, modify if currently in menu
    m_eCardState = cardState;
    m_eUserInput = userInput;

    // lock state in menu until complete
    if (m_bDeleteMenu)
    {
        // wait for new card to be detected on reader
        if(cardState == InputManager::NEW_KNOWN_CARD)
        {
            m_bDeleteReady = true;
        }
        m_eCardState = InputManager::DELETE_CARD_MENU;
    }

    if (m_bLinkMenu)
    {
        m_eCardState = InputManager::UNKNOWN_CARD_MENU;
    }
}

bool OutputManager::runDispatcher()
{
    // Check for index out of bounds
    if ((InputManager::NO_CARD > m_eCardState) ||
        (m_eCardState >= InputManager::NUMBER_OF_CARD_STATES) ||
        (UserInput::NO_ACTION > m_eUserInput) ||
        (m_eUserInput >= UserInput::NUMBER_OF_REQUESTS))
    {
        return false;
    }
    // to not clutter dispatcher
    handleErrors(); // checks for and handles card/userInput errors

    // initialize 2D-array of function pointers to address state-event transitions
    // dispatch table contains function pointers
    // cardStates = LINES, userInput = ROWS

    static const dispatcher dispatchTable[InputManager::NUMBER_OF_CARD_STATES - 2]
                                         [UserInput::NUMBER_OF_REQUESTS - 1] =
                                             {
                                                 //NOACT, PL_PS, PP_LP, NEXT_, PREV_, INC_V, DEC_V, ERROR
                                                 {&none, &help, &delt, &none, &none, &none, &none}, // NO_CARD
                                                 {&none, &plPs, &none, &next, &prev, &incV, &decV}, // ACTIVE_KNOWN_CARD,
                                                 {&link, &link, &link, &link, &link, &link, &link}, // NEW_KNOWN_CARD,
                                                 {&none, &conf, &abrt, &next, &prev, &none, &none}, // UNKNOWN_CARD_MENU,
                                                 {&none, &delC, &delA, &none, &none, &none, &none}, // DELETE_CARD_MENU,
                                             };
    dispatcher dispatchExecutor = dispatchTable[m_eCardState][m_eUserInput];
    (this->*dispatchExecutor)();
    // TODO: optimize dispatcher (link, erro!)
    return true;
}

// TODO: Add Debug output?
void OutputManager::handleErrors()
{
    if (m_eCardState == InputManager::ERROR_CARD_NOREAD)
    {
        m_mp3->play_specific_file(MSG_ERROR_CARDREAD);
        m_mp3->dont_skip_current_track();
        m_eCardState = InputManager::NO_CARD;
    }
    if (m_eCardState == InputManager::ERROR_CARD_NOWRITE)
    {
        m_mp3->play_specific_file(MSG_ERROR_CARDWRITE);
        m_mp3->dont_skip_current_track();
        m_eCardState = InputManager::NO_CARD;
    }

    if (m_eUserInput == UserInput::ERROR)
    {
        m_mp3->play_specific_file(MSG_ERROR_USERINPUT);
        m_mp3->dont_skip_current_track();
        m_eUserInput = UserInput::NO_ACTION;
    }
}

void OutputManager::delt()
{
    m_mp3->play_specific_file(MSG_DELETETAG);
    m_mp3->dont_skip_current_track();
    m_bDeleteMenu = true; // keep in delete menu
}

void OutputManager::delC()
{
    if(m_bDeleteReady)
    {
        
    }
}



void OutputManager::handleInputs(InputManager::eCardState cardState,
                                 UserInput::UserRequest_e userInput)
{
    switch (cardState)
    {
        mp3.loop();
    case InputManager::NO_CARD:
        if (!m_deleteCardRequested)
        {
            // Set LED&keepAlive playback
            // Allow certain mp3 commands
            handle_playPause = &(Mp3PlayerControl::play_pause);
        }
        else
        {
            // Set LED&keepAlive delete
            // Allow abort
            // Start timeout
        }
        break;
    case InputManager::ACTIVE_KNOWN_CARD:
        // Set LED&keepAlive playback
        // Allow all mp3 commands
        break;
    case InputManager::NEW_KNOWN_CARD:
        if (!m_deleteCardRequested)
        {
            // read card
            // link folder
            // play card
            // allow all mp3 commands
        }
        else
        {
            // ask for confirmation
            // allow abort
            // delete card
            //deleteCardRequested = false;
        }

        break;
    case InputManager::UNKNOWN_CARD:
        // set LED&keepAlive newCard
        // play voice menu
        // link folder to card
        break;

    default:
        // play error message
        // CARD_ERROR

        break;
    }
}

void OutputManager::handleUserInput(UserInput::UserRequest_e userInput)
{
    if (userInput == UserInput::DelCard)
    {
        m_deleteCardRequested = true;
    }

    switch (userAction)
    {
    case UserInput::NO_ACTION:
        break;
    case UserInput::PLAY_PAUSE:
        mp3.play_pause();
        break;
    case UserInput::NEXT_TRACK:
        mp3.next_track();
        break;
    case UserInput::PREV_TRACK:
        mp3.prev_track();
        break;
    case UserInput::INC_VOLUME:
        mp3.volume_up();
        break;
    case UserInput::DEC_VOLUME:
        mp3.volume_down();
        break;
    case UserInput::DelCard:
        break;
    case UserInput::Help:
        mp3.play_specific_file(MSG_HELP);
        break;
    case UserInput::Abort:
        mp3.play_specific_file(MSG_ABORTEED);
        break;
    case UserInput::Error:
#if DEBUGSERIAL
        usbSerial.com_println("UserInput: Error.");
#endif
        mp3.play_specific_file(MSG_ERROR);
        mp3.dont_skip_current_track();
        break;
    }
}

bool System::delete_link_tagToFolder()
{
    bool newCardPresent = false;
    handleKeepAliveLed.DELETE_CARD();
    UserInput::UserRequest_e userAction = UserInput::NO_ACTION;
    while (!newCardPresent) // TODO: ADD TIMEOUT!
    {
        // Wait for new card to be detected, meanwhile allow to abort
        aUserInput->set_card_detected(newCardPresent);
        userAction = aUserInput->get_user_request();
        handleMp3.DELETE_CARD(userAction);
        if (userAction == UserInput::Abort)
        {
            return; // deletion aborted
        }
        else if (userAction == UerInput::PLAY_PAUSE)
        {
            nfcTagReader.delete_card();
#if DEBUGSERIAL
            usbSerial.com_println("Card deleted");
#endif
            return;
        }
        newCardPresent = nfcTagReader.is_new_card_present();
    }
}

void HandleMp3::activeKnownCardPresent(UserRequest_e userAction)
{
    mp3.loop();
    switch (userAction)
    {
    case UserInput::NO_ACTION:
        break;
    case UserInput::PLAY_PAUSE:
        mp3.play_pause();
        break;
    case UserInput::NEXT_TRACK:
        mp3.next_track();
        break;
    case UserInput::PREV_TRACK:
        mp3.prev_track();
        break;
    case UserInput::INC_VOLUME:
        mp3.volume_up();
        break;
    case UserInput::DEC_VOLUME:
        mp3.volume_down();
        break;
    case UserInput::DelCard:
        break;
    case UserInput::Help:
        mp3.play_specific_file(MSG_HELP);
        break;
    case UserInput::Abort:
        mp3.play_specific_file(MSG_ABORTEED);
        break;
    case UserInput::Error:
#if DEBUGSERIAL
        usbSerial.com_println("UserInput: Error.");
#endif
        mp3.play_specific_file(MSG_ERROR);
        mp3.dont_skip_current_track();
        break;
    }
}

void HandleMp3::newKnownCardPresent(Folder &currentFolder)
{
    mp3.play_folder(currentFolder); //Folder setup and ready to play
}

void HandleMp3::newUnknownCardPresent()
{
    mp3.play_specific_file(MSG_UNKNOWNTAG);
    mp3.dont_skip_current_track();
}

void HandleMp3::DELETE_CARD(UserRequest_e userAction)
{
    switch (userAction)
    {
    case UserInput::NO_ACTION:
        break;
    case UserInput::DelCard:
        mp3.play_specific_file(MSG_DELETETAG); // prompt to have tag placed that shall be deleted.
        mp3.dont_skip_current_track();
        break;
    case UserINput::PLAY_PAUSE:
        mp3.play_specific_file(MSG_CONFIRMED); // TODO voice prompt
        mp3.dont_skip_current_track();
        break;
    }
}

void HandleMp3::CARD_ERROR()
{
    mp3.play_specific_file(MSG_ERROR);
    mp3.dont_skip_current_track();
}

void HandleKeepAliveLed::playback()
{
    static bool wasPlaying = true; // init true to make check on startup
    if (wasPlaying == isPlaying)
    {
        return; // no status change.
    }

    if (isPlaying)
    {
        aKeepAlive.set_idle_timer(false);
        aLed.set_led_behavior(StatusLed::solid);
    }
    else
    {
        aKeepAlive.set_idle_timer(true);
        aLed.set_led_behavior(StatusLed::flash_slow);
    }
    wasPlaying = isPlaying;
}

void HandleKeepAliveAndLed::newCard()
{
    aKeepAlive.set_idle_timer(false);
    aLed.set_led_behavior(StatusLed::dim);
}

void HandleKeepAliveAndLed::deleteCard()
{
    aKeepAlive.set_idle_timer(false);
    aLed.set_led_behavior(StatusLed::flash_quick);
}

/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
establish_link_tagToFolder() asks the user to select a folder on the SD card 
and to specify a playmode
*/
bool System::establish_link_tagToFolder()
{

    // Get folder number on SD card
    uint8_t folderId = 0;
    uint8_t trackCount = 0;
    Folder::ePlayMode playMode = Folder::UNDEFINED;
    Folder tempFolder;
    // user input: Which folder to link?
    folderId = voice_menu(
        99,
        MSG_UNKNOWNTAG,
        false,
        true,
        0);
    // user input: Which play mode?
    playMode = static_cast<Folder::ePlayMode>(voice_menu(
        static_cast<uint8_t>(Folder::ePlayMode::ENUM_COUNT),
        MSG_TAGLINKED,
        true,
        false,
        static_cast<uint8_t>(Folder::ePlayMode::ALBUM)));
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

currentFolder.setup_dependencies(&eeprom, init_random_generator());

void System::playback_handle_user_input(UserInput &aUserInput, Mp3PlayerControl &mp3, bool cardPresent)
{
    // Tell interface if a known card is present.
    aUserInput->set_card_detected(cardPresent);
    UserInput::UserRequest_e userAction = aUserInput->get_user_request();
    switch (userAction)
    {
    case UserInput::NO_ACTION:
        break;
    case UserInput::PLAY_PAUSE:
        mp3.play_pause();
        break;
    case UserInput::NEXT_TRACK:
        mp3.next_track();
        break;
    case UserInput::PREV_TRACK:
        mp3.prev_track();
        break;
    case UserInput::INC_VOLUME:
        mp3.volume_up();
        break;
    case UserInput::DEC_VOLUME:
        mp3.volume_down();
        break;
    case UserInput::DelCard:
        mp3.play_specific_file(MSG_DELETETAG); // prompt to have tag placed that shall be deleted.
        mp3.dont_skip_current_track();
        deleteCardRequested = true;
    case UserInput::Help:
        mp3.play_specific_file(MSG_HELP);
        break;
    case UserInput::Abort:
        mp3.play_specific_file(MSG_ABORTEED);
        break;
    case UserInput::Error:
#if DEBUGSERIAL
        usbSerial.com_println("UserInput: Error.");
#endif
        mp3.play_specific_file(MSG_ERROR);
        mp3.dont_skip_current_track();
        break;
    }
}

uint8_t handle_user_input_folderId(uint8_t numberOfOptions, uint16_t startMessage, bool returnValuesOffsetStartMessage,
                                   bool previewSelectedFolder, uint8_t defaultValue)
{
    uint8_t returnValue = defaultValue;
    uint8_t lastReturnValue = defaultValue;
    uint16_t messageOffset = (uint8_t)returnValuesOffsetStartMessage * startMessage;
    UserInput::UserRequest_e userAction = UserInput::NO_ACTION;
    if (startMessage == 0)
    {
        return defaultValue; // invalid function call
    }
    // Stop track if playing
    if (mp3.is_playing())
    {
        mp3.play_pause();
    }
    mp3.play_specific_file(startMessage);
    mp3.dont_skip_current_track();
#if DEBUGSERIAL
    usbSerial.com_println("voiceMenu() option: ");
    usbSerial.com_print(numberOfOptions);
#endif
    aUserInput->set_card_detected(true); // needed to detect Abort signal
    // Play prompt and detect user input
    while (true)
    {
        lastReturnValue = returnValue;
        userAction = aUserInput->get_user_request();
        mp3.loop();
        switch (userAction)
        {
        case UserInput::NO_ACTION:
            break;
        case UserInput::Abort:
#if DEBUGSERIAL
            usbSerial.com_println("Aborted!");
#endif
            mp3.play_specific_file(MSG_ABORTEED);
            return defaultValue;
        case UserInput::PLAY_PAUSE:
            if (returnValue != 0)
            {
#if DEBUGSERIAL
                usbSerial.com_print(returnValue);
#endif
                return returnValue;
            }
            break;
        case UserInput::NEXT_TRACK:
            returnValue = min(returnValue + 1, numberOfOptions);
            break;
        case UserInput::PREV_TRACK:
            returnValue = max(returnValue - 1, 1);
            break;
        default:
            break;
        }
        if (lastReturnValue != returnValue)
        {
            // play number of current choice, e.g. "one".
            mp3.play_specific_file(messageOffset + returnValue);
            mp3.dont_skip_current_track();
            if (previewSelectedFolder)
            {
                // Preview: Play first track from folder to link
                Folder previewFolder = Folder(returnValue, Folder::ONELARGETRACK, 1);
                previewFolder.setup_dependencies(&eeprom, 0);
                mp3.play_folder(&previewFolder);
            }
        }
    }
}

// dependencies: setup_folder(), mp3 (pausing), nfctagreader (write to card), delay
void setup_card()
{
#if DEBUGSERIAL
    usbSerial.com_println("setup_card()");
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
    delayControl.delay_ms(WAIT_DFMINI_READY);
}
void System::handleCardState()
{
    bool cardPresent = nfcTagReader.is_card_present();
    aUserInput->set_card_detected(cardPresent);

    handleKeepAliveLed.playback() if (cardPresent)
    {
        if (!nfcTagReader.is_new_card_present())
        {
            return ACTIVE_KNOWN_CARD;
            if (deletionRequested)
            {
                delete_link_tagToFolder(); // card cannot be fully read: clear
                deletionRequested = false;
            }
            else
            {
                // Active Card present Accept user input for playback control.
                handleMp3.activeKnownCardPresent(userAction); // Autoplay and DFmini loop
            }
        }
        else // New card detected: runs once as new card is automatically set to ActiveCard
        {

            handleKeepAliveLed.newCard();
            if (nfcTagReader.read_folder_from_card(currentFolder))
            {
                return NEW_KNOWN_CARD;
                currentFolder.setup_dependencies(&eeprom, init_random_generator());
                handleMp3.newKnownCardPresent(currentFolder);
            }
            else
            {
                if (!nfcTagReader.is_known_card())
                {
                    return UNKNOWN_CARD;
                    handleMp3.newUnknownCardPresent();
                    establish_link_tagToFolder(); // unknown card: configure
                }
                else
                {
                    return CARD_ERROR;
                    handleMp3.CARD_ERROR();
                    delete_link_tagToFolder(); // card cannot be fully read: clear
                }
            }
        }
    }
    else
    {
        NO_CARD;
    }
}