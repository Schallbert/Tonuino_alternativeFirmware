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
    if (m_pMenuInstance != nullptr)
    {
        result = m_pMenuInstance->isActive();
    }
    return result;
}

bool VoiceMenu::isComplete()
{
    bool result{false};
    if (m_pMenuInstance != nullptr)
    {
        result = m_pMenuInstance->isComplete();
    }
    return result;
}

void VoiceMenu::loop()
{
    if (!isActive())
    {
        checkEnterLinkMenu();
        checkEnterDeleteMenu();
    }

    if (isActive())
    {
        dispatchInputs();
        checkPlayPrompt();
        checkPlayFolderPreview();
        //checkLeaveMenu();
    }
}

void VoiceMenu::checkEnterLinkMenu()
{
    if (m_inputState.tagState == Nfc_interface::NEW_UNKNOWN_TAG)
    {
        m_pMenuInstance = Menu_factory::getInstance(Menu_factory::LINK_MENU);
        m_pMenuInstance->confirm();
    }
}

void VoiceMenu::checkEnterDeleteMenu()
{
    if (m_inputState.tagState == Nfc_interface::ACTIVE_KNOWN_TAG &&
        m_inputState.btnState == UserInput::PP_LONGPRESS)
    {
        m_pMenuInstance = Menu_factory::getInstance(Menu_factory::DELETE_MENU);
        m_pMenuInstance->confirm();
    }
}

void VoiceMenu::checkLeaveMenu()
{
    if (isComplete())
    {
        delete m_pMenuInstance;
        // TODO: make Nfc save folder info to Tag
    }
}

void VoiceMenu::checkPlayPrompt()
{
    VoicePrompt prompt = m_pMenuInstance->getPrompt();
    m_pPromptPlayer->checkPlayPrompt(prompt);
}

void VoiceMenu::checkPlayFolderPreview()
{
    if (m_pMenuInstance->isPreviewAvailable())
    {
        Folder previewFolder = m_pMenuInstance->getFolderInformation();
        m_pPromptPlayer->playFolderPreview(previewFolder);
    }
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
