#include "System.h"

void System::setup()
{
    m_KeepAlive.keep_alive(); //Activate KeepAlive to maintain power supply to circuits
    // Initializes all objects needed
    


#if DEBUGSERIAL
    usbSerial.com_begin(9600); // Some debug output via serial
    usbSerial.com_println("Booting");
#endif

    aLed.set_led_behavior(StatusLed::solid);
    //Init Timer1 for Encoder read
    //init UserInput
    aUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    aUserInput->init();

#if DEBUGSERIAL
    usbSerial.com_println("Started.");
#endif
}

uint32_t System::initRandomGenerator()
{
    uint32_t ADC_LSB;
    uint32_t ADCSeed;
    for (uint8_t i = 0; i < 128; i++)
    {
        ADC_LSB = pinControl.analog_read(PINANALOG_RNDMGEN) & 0x1;
        ADCSeed ^= ADC_LSB << (i % 32);
    }
    return ADCSeed; // Init Arduino dependencies generator
}

InputManager::eCardState InputManager::getCardState()
{
    bool cardPresent = m_nfcTagReader->is_card_present();
    m_userInput.set_card_detected(cardPresent); // TODO: needed?

    if (cardPresent)
    {
        if (!m_nfcTagReader->is_new_card_present())
        {
            return ACTIVE_KNOWN_CARD;
        }
        else // New card detected: runs once as new card is automatically set_state to ActiveCard
        {
            Folder dummyFolder;
            if (m_nfcTagReader->read_folder_from_card(dummyFolder))
            {
                return NEW_KNOWN_CARD;
            }
            else // New card but folder cannot be read
            {
                if (!m_nfcTagReader->is_known_card())
                {
                    return UNKNOWN_CARD_MENU;
                }
            }
        }
    }
    else // No card present
    {
        return NO_CARD;
    }
}

UserInput::UserRequest_e InputManager::getUserInput()
{
    return m_userInput.get_user_request();
}

void OutputManager::setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput)
{
    // set_state to input values, modify if currently in menu
    m_eCardState = cardState;
    m_eUserInput = userInput;

    m_sysPwr.set_playback(m_mp3->is_playing());

    // lock state in menu, waiting for card placing that shall be deleted
    if ((m_deleteMenu.get_state(DeleteMenu::DELETE_MENU)) &&
        (cardState == InputManager::NEW_KNOWN_CARD))
    {
        m_deleteMenu.set_state(DeleteMenu::DELETE_READY);
    }

    if (!m_deleteMenu.get_state(DeleteMenu::NO_MENU))
    {
        m_eCardState = InputManager::DELETE_CARD_MENU; // delete menu entered
        m_sysPwr.set_delMenu(true);
    }

    if (cardState == InputManager::UNKNOWN_CARD_MENU)
    {
        m_linkMenu.set_state(true); // runs card link method on UNKNOWN_CARD detected
        m_sysPwr.set_delMenu(false);
    }

    if (m_linkMenu.get_state())
    {
        m_eCardState = InputManager::UNKNOWN_CARD_MENU; // keeps in link menu
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
    handleErrors(); // TODO: refactor?

    // initialize 2D-array of function pointers to address state-event transitions
    // dispatch table contains function pointers
    // cardStates = ROWS, userInput = COLUMNS
    static const dispatcher dispatchTable[InputManager::NUMBER_OF_CARD_STATES]
                                         [UserInput::NUMBER_OF_REQUESTS - 1] =
                                             {
                                                 //NOAC, PL_PS, PP_LP, NEXT_, PREV_, INC_V, DEC_V, ERROR
                                                 {&none, &plPs, &help, &next, &prev, &incV, &decV}, // NO_CARD
                                                 {&none, &plPs, &delt, &next, &prev, &incV, &decV}, // ACTIVE_KNOWN_CARD,
                                                 {&none, &plPs, &none, &next, &prev, &incV, &decV}, // NEW_KNOWN_CARD,
                                                 {&none, &linC, &abrt, &linN, &linP, &none, &none}, // UNKNOWN_CARD_MENU,
                                                 {&none, &delC, &abrt, &none, &none, &none, &none}, // DELETE_CARD_MENU,
                                             };
    dispatcher dispatchExecutor = dispatchTable[m_eCardState][m_eUserInput];
    (this->*dispatchExecutor)();
    // TODO: optimize dispatcher (link, erro!)
    return true;
}

void OutputManager::handleErrors()
{
    // TODO: refactor?
    if (m_eUserInput == UserInput::ERROR)
    {
        m_mp3->play_specific_file(MSG_ERROR_USERINPUT);
        m_mp3->dont_skip_current_track();
        m_eUserInput = UserInput::NO_ACTION;
    }
}

void OutputManager::read()
{
    if (m_nfcTagReader->read_folder_from_card(m_currentFolder))
    {
        m_currentFolder.setup_dependencies(&eeprom, init_random_generator()); // TODO: SOLVE maybe on top level?
        m_mp3->play_folder(&m_currentFolder);
    }
    else
    {
        // TODO: Add Debug Output?
        m_mp3->play_specific_file(MSG_ERROR_CARDREAD);
        m_mp3->dont_skip_current_track();
        m_eCardState = InputManager::NO_CARD;
    }
}

void OutputManager::delt()
{
    m_mp3->play_specific_file(MSG_DELETETAG);
    m_mp3->dont_skip_current_track();
    m_deleteMenu.set_state(DeleteMenu::DELETE_MENU); // keep in delete menu
}

void OutputManager::delC()
{
    if (m_deleteMenu.get_state(DeleteMenu::DELETE_READY))
    {
        // Do delete the card.
        m_mp3->play_specific_file(MSG_CONFIRMED);
        m_deleteMenu.set_state(DeleteMenu::NO_MENU);
        m_nfcTagReader->erase_card();
    }
    else
    {
        m_mp3->play_specific_file(MSG_DELETETAG);
    }
}

void OutputManager::abrt()
{
    m_deleteMenu.set_state(DeleteMenu::NO_MENU); // reset menu state
    m_linkMenu.set_state(false);
    m_mp3->play_specific_file(MSG_ABORTEED);
}

void OutputManager::linC()
{
    if (m_linkMenu.select_confirm())
    {
        // link folder information complete! Obtain folder and save to card.
        m_currentFolder = m_linkMenu.get_folder();
        m_linkMenu.set_state(false);
        if (m_nfcTagReader->write_folder_to_card(m_currentFolder))
        {
            m_mp3->play_specific_file(MSG_TAGCONFSUCCESS);
            m_mp3->dont_skip_current_track();
            read();
        }
        else
        {
            m_mp3->play_specific_file(MSG_ERROR);
            m_mp3->dont_skip_current_track();
            abrt();
        }
    }
}

void OutputManager::linN()
{
    m_linkMenu.select_next();
}

void OutputManager::linP()
{
    m_linkMenu.select_prev();
}

LinkMenu::LinkMenu(Mp3PlayerControl *mp3)
{
    m_mp3 = mp3;
}

void LinkMenu::set_state(bool linkMenu)
{
    if (linkMenu)
    {
        if (!m_bMenuState)
        {
            init();
        }
    }
    else
    {
        leave();
    }
}

void LinkMenu::init()
{
    m_bMenuState = true;
    m_bLinkState = false;
    m_ui8Option = 0;
    m_ui8OptionRange = MAXFOLDERCOUNT;

    //mp3.loop();
    if (m_mp3->is_playing())
    {
        m_mp3->play_pause();
    }
    m_mp3->play_specific_file(MSG_UNKNOWNTAG);
}

void LinkMenu::leave()
{
    m_bMenuState = false;
    m_bLinkState = false;
    m_ui8Option = 0;
    m_ui8OptionRange = 0;
}

bool LinkMenu::select_confirm()
{
    static uint8_t folderId = 0;
    if (!m_bLinkState)
    {
        // Confirms folder selection
        folderId = m_ui8Option;
        m_ui8Option = 0;
        m_ui8OptionRange = static_cast<uint8_t>(Folder::ePlayMode::ENUM_COUNT);
        m_bLinkState = true;
        m_mp3->play_specific_file(MSG_TAGLINKED); // Tell user to select playMode
    }
    else
    {
        // Confirms playmode selection.
        Folder::ePlayMode playMode = static_cast<Folder::ePlayMode>(m_ui8Option);
        uint8_t trackCount = m_mp3->get_trackCount_of_folder(folderId);
        // Create new folder object and copy to main's folder object
        Folder tempFolder = Folder(folderId, playMode, trackCount);
        if (tempFolder.is_initiated())
        {
            m_linkedFolder = tempFolder; // Success! copy temporary folder to new folder.
            return true;
        }
        else
        {
            m_mp3->play_specific_file(MSG_ERROR_FOLDER);
            m_mp3->dont_skip_current_track();
        }
    }
    return false;
}

void LinkMenu::select_next()
{
    // conditionally increment option (rollover)
    ++m_ui8Option;
    if (m_ui8Option > m_ui8OptionRange)
    {
        m_ui8Option = 1;
    }

    play_voice_prompt();
}

void LinkMenu::select_prev()
{
    // conditionally decrement option (rollover)
    --m_ui8Option;
    if (m_ui8Option > m_ui8OptionRange)
    {
        m_ui8Option = m_ui8OptionRange;
    }

    play_voice_prompt();
}

void LinkMenu::play_voice_prompt()
{
    //mp3.loop(); // TODO: Check if needed
    if (!m_bLinkState)
    {
        // play folderId of current choice, e.g. "one".
        m_mp3->play_specific_file(static_cast<uint16_t>(m_ui8Option));
        m_mp3->dont_skip_current_track();

        // play preview of selected folder contents
        Folder previewFolder = Folder(m_ui8Option, Folder::ONELARGETRACK, 1);
        // TODO: SOLVE SOMEWHERE ELSE?!
        previewFolder.setup_dependencies(&eeprom, 0);
        m_mp3->play_folder(&previewFolder);
    }
    else
    {
        // Prompt selected playMode
        m_mp3->play_specific_file(static_cast<uint16_t>(MSG_TAGLINKED + m_ui8Option));
        m_mp3->dont_skip_current_track();
    }
}

Folder LinkMenu::get_folder()
{
    return m_linkedFolder;
}

void KeepAlive_StatusLed::set_playback(bool isPlaying)
{
    if (isPlaying)
    {
        m_keep.set_idle_timer(false); // Playing
        m_ed.set_led_behavior(StatusLed::solid);
    }
    else
    {
        m_keep.set_idle_timer(true); // Paused
        m_led.set_led_behavior(StatusLed::flash_slow);
    }
}

void KeepAlive_StatusLed::set_delMenu(bool isDelMenu)
{
    m_keep.set_idle_timer(false);
    if (isDelMenu)
    {
        m_led.set_led_behavior(StatusLed::flash_quick); // Delete Menu
    }
    else
    {
        m_led.set_led_behavior(StatusLed::flash_slow); // Link Menu
    }
}