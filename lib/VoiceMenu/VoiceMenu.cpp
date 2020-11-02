#include "VoiceMenu.h"

VoiceMenu::~VoiceMenu()
{
    if (m_pMenuInstance)
    {
        delete m_pMenuInstance;
    }
}

void VoiceMenu::setInputState(InputState inputState)
{
    m_inputState = inputState;
}

bool VoiceMenu::isActive()
{
    bool result{false};
    if (m_pMenuInstance)
    {
        result = m_pMenuInstance->isActive();
    }
    return result;
}

void VoiceMenu::loop()
{
    checkEnterLinkMenu();
    checkEnterDeleteMenu();

    if (isActive())
    {
        dispatchInputs();
        checkPlayPrompt();
    }
}

void VoiceMenu::checkEnterLinkMenu()
{
    if (m_inputState.tagState == Nfc_interface::NEW_UNKNOWN_TAG)
    {
        setMenuInstance(Menu_factory::LINK_MENU);
        m_pMenuInstance->confirm();
    }
}

void VoiceMenu::checkEnterDeleteMenu()
{
    if (m_inputState.tagState == Nfc_interface::ACTIVE_KNOWN_TAG &&
        m_inputState.btnState == UserInput::PP_LONGPRESS)
    {
        setMenuInstance(Menu_factory::DELETE_MENU);
        m_pMenuInstance->confirm();
    }
}

void VoiceMenu::setMenuInstance(Menu_factory::eMenuType menu)
{
    if (m_pMenuInstance)
    {
        delete m_pMenuInstance;
    }
    m_pMenuInstance = Menu_factory::getInstance(menu);
}

void VoiceMenu::checkPlayPrompt()
{
    VoicePrompt prompt = m_pMenuInstance->getPrompt();

    m_pPromptPlayer->checkPlayPrompt(prompt);
}

void VoiceMenu::dispatchInputs()
{
    typedef VoiceMenu VM;
    static const dispatcher dispatchTable[UserInput::NUMBER_OF_REQUESTS] =
        {
            //NOAC,     PL_PS,     PP_LP,     NEXT_,     PREV_,     INC_V,     DEC_V,
            &VM::none, &VM::conf, &VM::abrt, &VM::next, &VM::prev, &VM::none, &VM::none};
    dispatcher dispatchExecutor = dispatchTable[m_inputState.btnState];
    (this->*dispatchExecutor)();
}

/*

void VoiceMenu::checkPlayPrompt()
{
    Menu_interface::VoicePrompt prompt = m_pMenuInstance.getPrompt();
    m_Mp3Ctrl.playSpecificFile(prompt.promptId);
    if (!prompt.allowSkip)
    {
        m_Mp3Ctrl.dontSkipCurrentTrack();
    }
}
*/
