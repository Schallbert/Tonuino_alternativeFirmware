#include "Tonuino_config.h"
#include "SimpleTimer/SimpleTimer.h"

#include "VoiceMenu.h"

void VoiceMenu::setUserInput(Message::eMessageContent input)
{
    m_userInput = input;
}

void VoiceMenu::setTagState(Message::eMessageContent input)
{
    m_tagState = input;
}

void VoiceMenu::loop()
{
    if (isActive())
    {
        handlePromptStatus();
        dispatchInputs();
        m_pMenuInstance->setStatusLed();
        m_pMenuInstance->setTagState(m_tagState);
        checkTimerElapsed();
        checkLeaveMenu();
    }
    else
    {
        checkEnterLinkMenu();
        checkEnterDeleteMenu();
    }
}

void VoiceMenu::handlePromptStatus()
{
    if ((m_userInput != Message::INPUTNONE))
    {
        m_rMp3Prompt.stopPrompt();
    }
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

void VoiceMenu::checkEnterLinkMenu()
{
    if (m_tagState == Message::UNKNOWNTAG)
    {
        m_pMenuInstance = m_MenuFactory.getInstance(Menu_factory::LINK_MENU,
                                                    m_rNfcControl,
                                                    m_rMp3Play,
                                                    m_rMp3Prompt,
                                                    m_rMessageHandler,
                                                    m_rPowerManager);
        enterMenu();
    }
}

void VoiceMenu::enterMenu()
{
    if (m_pMenuInstance != nullptr)
    {
        m_pMenuInstance->confirm();
    }
    m_rMenuTimer.start(MENU_TIMEOUT_SECS);
}

void VoiceMenu::checkEnterDeleteMenu()
{
    if ((m_tagState == Message::NOTAG) &&
        (m_userInput == Message::INPUTPLPSDC))
    {
        m_pMenuInstance = m_MenuFactory.getInstance(Menu_factory::DELETE_MENU,
                                                    m_rNfcControl,
                                                    m_rMp3Play,
                                                    m_rMp3Prompt,
                                                    m_rMessageHandler,
                                                    m_rPowerManager);
        enterMenu();
    }
}

void VoiceMenu::checkLeaveMenu()
{
    // Could be completed or aborted
    if (isComplete() || !isActive())
    {
        m_rMenuTimer.stop();
        m_pMenuInstance = nullptr;
    }
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

void VoiceMenu::dispatchInputs()
{
    typedef VoiceMenu VM;
    static const dispatcher dispatchTable[IN_REQUEST_OPTIONS] =
        {
            &VM::none, &VM::conf, &VM::abrt, &VM::none, &VM::next, &VM::none, &VM::prev, &VM::none};
    dispatcher dispatchExecutor = dispatchTable[static_cast<uint8_t>(m_userInput) & 0x0F]; //
    (this->*dispatchExecutor)();

    //restart menu timer on button press
    if (m_userInput != Message::INPUTNONE)
    {
        m_rMenuTimer.stop();
        m_rMenuTimer.start(MENU_TIMEOUT_SECS);
    }
}

void VoiceMenu::checkTimerElapsed()
{
    if (m_rMenuTimer.isElapsed())
    {
        abrt();
    }
}
