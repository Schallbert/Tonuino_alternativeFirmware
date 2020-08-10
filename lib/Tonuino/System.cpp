#include "System.h"

System::System()
{
    // Initializes all objects needed
    m_pPwrCtrl = new KeepAlive_StatusLed();
    m_pPwrCtrl->setup();

#if DEBUGSERIAL
    m_pUsbSerial->com_begin(9600); // Some debug output via serial
    m_pUsbSerial->com_println("Booting");
#endif

    // Set dependency objects ------------------------------
    m_pPinControl = new Arduino_pins();
    m_pUsbSerial = new Arduino_com();
    m_pDelayControl = new Arduino_delay();
    m_pReader = new Mfrc522();
    m_pNfcTagReader = new NfcTag(m_pReader); // Constructor injection of concrete reader
    m_pDfMini = new DfMini();
    m_pMenuTimer = new MenuTimer();
    m_pMp3 = new Mp3PlayerControl(m_pDfMini, m_pPinControl, m_pUsbSerial, m_pDelayControl);
    m_pUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);

    // Initialize objects if needed ------------------------
    //init UserInput
    m_pUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    m_pUserInput->init();

#if DEBUGSERIAL
    m_pUsbSerial->com_println("Started.");
#endif
}

System::~System()
{

#if DEBUGSERIAL
    m_pUsbSerial->com_println("Shutdown");
#endif

    // delete dependency objects
    delete m_pPinControl;
    delete m_pUsbSerial;
    delete m_pDelayControl;
    delete m_pReader;
    delete m_pNfcTagReader;
    delete m_pDfMini;
    delete m_pMp3;
    delete m_pMenuTimer;
    m_pUserInput = nullptr;

    // finally shut down system
    m_pPwrCtrl->request_shutdown();
    m_pPwrCtrl->allow_shutdown();
    delete m_pPwrCtrl;
}

bool System::loop()
{
    InputManager::eCardState cardState = m_inputManager.getCardState();
    UserInput::UserRequest_e userEvent = m_inputManager.getUserInput();
    m_outputManager.setInputStates(cardState, userEvent);
    m_outputManager.runDispatcher();
    if (m_pPwrCtrl->get_shutdown_request())
    {
        return false;
    }
    return true;
}

void System::timer1_task_1ms()
{
    static volatile uint16_t ui16Ticks = 0;

    m_pPwrCtrl->notify_timer_task();       // idle timer
    m_pUserInput->userinput_service_isr(); // userInput service

    ++ui16Ticks;
    if (ui16Ticks >= 1000) // 1ms --> 1s
    {
        ui16Ticks = 0;
        timer1_task_1s();
    }
}

void System::timer1_task_1s()
{
    m_pMp3->lullabye_timeout_tick1s();
    m_pMenuTimer->timer_tick1s();
}

uint32_t InputManager::getRandomSeed()
{
    uint32_t ADC_LSB;
    uint32_t ADCSeed;
    for (uint8_t i = 0; i < 128; i++)
    {
        ADC_LSB = m_pPinControl->analog_read(PINANALOG_RNDMGEN) & 0x1;
        ADCSeed ^= ADC_LSB << (i % 32);
    }
    return ADCSeed; // Init Arduino random generator
}

InputManager::eCardState InputManager::getCardState()
{
    bool cardPresent = m_pNfcTagReader->is_card_present();
    m_pUserInput->set_card_detected(cardPresent); // TODO: needed?

    if (cardPresent)
    {
        if (!m_pNfcTagReader->is_new_card_present())
        {
            return ACTIVE_KNOWN_CARD;
        }
        else // New card detected: runs once as new card is automatically set_state to ActiveCard
        {
            Folder dummyFolder;
            if (m_pNfcTagReader->read_folder_from_card(dummyFolder))
            {
                return NEW_KNOWN_CARD;
            }
            else // New card but folder cannot be read
            {
                if (!m_pNfcTagReader->is_known_card())
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
    return m_pUserInput->get_user_request();
}

// TODO: Refactor (switch?)
void OutputManager::setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput)
{
    // set_state to input values, modify if currently in menu
    m_eCardState = cardState;
    m_eUserInput = userInput;

    m_pSysPwr->set_playback(m_pMp3->is_playing());

    // lock state in menu, waiting for card placing that shall be deleted
    if ((m_deleteMenu.get_state(DeleteMenu::DELETE_MENU)) &&
        (cardState == InputManager::NEW_KNOWN_CARD))
    {
        m_deleteMenu.set_state(DeleteMenu::DELETE_READY);
    }

    if (!m_deleteMenu.get_state(DeleteMenu::NO_MENU))
    {
        m_eCardState = InputManager::DELETE_CARD_MENU; // delete menu entered
        m_pSysPwr->set_delMenu();
        m_pMenuTimer->set_active(true);
    };

    if (cardState == InputManager::UNKNOWN_CARD_MENU)
    {
        m_linkMenu.set_state(true); // runs card link method on UNKNOWN_CARD detected
        m_pMenuTimer->set_active(true);
        m_pSysPwr->set_linkMenu();
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
        m_pMp3->play_specific_file(MSG_ERROR_USERINPUT);
        m_pMp3->dont_skip_current_track();
        m_eUserInput = UserInput::NO_ACTION;
    }
}

void OutputManager::read()
{
    if (m_pNfcTagReader->read_folder_from_card(m_currentFolder))
    {
        m_currentFolder.setup_dependencies(&m_eeprom, m_ui32RandomSeed); // TODO: SOLVE maybe on top level?
        m_pMp3->play_folder(&m_currentFolder);
    }
    else
    {
        // TODO: Add Debug Output?
        m_pMp3->play_specific_file(MSG_ERROR_CARDREAD);
        m_pMp3->dont_skip_current_track();
        m_eCardState = InputManager::NO_CARD;
    }
}

void OutputManager::delt()
{
    m_pMp3->play_specific_file(MSG_DELETETAG);
    m_pMp3->dont_skip_current_track();
    m_deleteMenu.set_state(DeleteMenu::DELETE_MENU); // keep in delete menu
}

void OutputManager::delC()
{
    if (m_deleteMenu.get_state(DeleteMenu::DELETE_READY))
    {
        m_pMenuTimer->set_active(false);
        // Do delete the card.
        m_pMp3->play_specific_file(MSG_CONFIRMED);
        m_deleteMenu.set_state(DeleteMenu::NO_MENU);
        m_pNfcTagReader->erase_card();
    }
    else
    {
        m_pMp3->play_specific_file(MSG_DELETETAG);
    }
}

void OutputManager::abrt()
{
    m_pMenuTimer->set_active(false);
    m_deleteMenu.set_state(DeleteMenu::NO_MENU); // reset menu state
    m_linkMenu.set_state(false);
    m_pMp3->play_specific_file(MSG_ABORTEED);
}

void OutputManager::linC()
{
    if (m_linkMenu.select_confirm())
    {
        m_pMenuTimer->set_active(false);
        // link folder information complete! Obtain folder and save to card.
        m_currentFolder = m_linkMenu.get_folder();
        m_linkMenu.set_state(false);
        if (m_pNfcTagReader->write_folder_to_card(m_currentFolder))
        {
            m_pMp3->play_specific_file(MSG_TAGCONFSUCCESS);
            m_pMp3->dont_skip_current_track();
            read();
        }
        else
        {
            m_pMp3->play_specific_file(MSG_ERROR);
            m_pMp3->dont_skip_current_track();
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
    if (m_pMp3->is_playing())
    {
        m_pMp3->play_pause();
    }
    m_pMp3->play_specific_file(MSG_UNKNOWNTAG);
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
    if (!m_bLinkState)
    {
        // Confirms folder selection
        m_ui8FolderId = m_ui8Option;
        m_ui8Option = 0;
        m_ui8OptionRange = static_cast<uint8_t>(Folder::ePlayMode::ENUM_COUNT);
        m_bLinkState = true;
        m_pMp3->play_specific_file(MSG_TAGLINKED); // Tell user to select playMode
    }
    else
    {
        // Confirms playmode selection.
        Folder::ePlayMode playMode = static_cast<Folder::ePlayMode>(m_ui8Option);
        uint8_t trackCount = m_pMp3->get_trackCount_of_folder(m_ui8FolderId);
        // Create new folder object and copy to main's folder object
        Folder tempFolder = Folder(m_ui8FolderId, playMode, trackCount);
        if (tempFolder.is_initiated())
        {
            m_linkedFolder = tempFolder; // Success! copy temporary folder to new folder.
            return true;
        }
        else
        {
            m_pMp3->play_specific_file(MSG_ERROR_FOLDER);
            m_pMp3->dont_skip_current_track();
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
        m_pMp3->play_specific_file(static_cast<uint16_t>(m_ui8Option));
        m_pMp3->dont_skip_current_track();

        // play preview of selected folder contents
        Folder previewFolder = Folder(m_ui8Option, Folder::ONELARGETRACK, 1);
        // TODO: SOLVE SOMEWHERE ELSE?!
        previewFolder.setup_dependencies(m_pEeprom, 0);
        m_pMp3->play_folder(&previewFolder);
    }
    else
    {
        // Prompt selected playMode
        m_pMp3->play_specific_file(static_cast<uint16_t>(MSG_TAGLINKED + m_ui8Option));
        m_pMp3->dont_skip_current_track();
    }
}

Folder LinkMenu::get_folder()
{
    return m_linkedFolder;
}

// counts menu timer if active
void MenuTimer::timer_tick1s()
{
    if (m_bActive)
    {
        ++m_ui16SecCount;
    }

    if (m_ui16SecCount >= MENU_TIMEOUT_SECS)
    {
        m_bElapsed = true;
    }
}

void MenuTimer::set_active(bool bActive)
{
    m_bActive = bActive;
}

bool MenuTimer::get_elapsed()
{
    if (m_bElapsed)
    {
        m_bElapsed = false;
        return true;
    }
    return false;
}

void KeepAlive_StatusLed::setup()
{
    m_keep.keep_alive(); //Activate KeepAlive to maintain power supply to circuits
    m_led.set_led_behavior(StatusLed::solid);
}

void KeepAlive_StatusLed::request_shutdown()
{
    m_led.set_led_behavior(StatusLed::off);
    m_keep.request_shutdown();
}

bool KeepAlive_StatusLed::get_shutdown_request()
{
    return m_keep.get_shutdown_request();
}

void KeepAlive_StatusLed::allow_shutdown()
{
    m_keep.allow_shutdown();
}

void KeepAlive_StatusLed::set_playback(bool isPlaying)
{
    if (isPlaying)
    {
        m_keep.set_idle_timer(false); // Playing
        m_led.set_led_behavior(StatusLed::solid);
    }
    else
    {
        m_keep.set_idle_timer(true); // Paused
        m_led.set_led_behavior(StatusLed::flash_slow);
    }
}

void KeepAlive_StatusLed::set_delMenu()
{
    m_keep.set_idle_timer(false);
    m_led.set_led_behavior(StatusLed::flash_quick); // Delete Menu
}

void KeepAlive_StatusLed::set_linkMenu()
{
    m_keep.set_idle_timer(false);
    m_led.set_led_behavior(StatusLed::flash_slow); // Link Menu
}

void KeepAlive_StatusLed::notify_timer_task()
{
    m_keep.idle_timer_tick1ms();
    m_led.led_service();
}