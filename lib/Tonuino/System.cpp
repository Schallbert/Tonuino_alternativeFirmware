#include "System.h"

#include "../UserInput/UserInput/UserInput_implementation.h"
#include "../UserInput/ClickEncoder/ClickEncoder_implementation.h"

System::System()
{
    //UserInput_factory m_pUserInputFactory{};
    //m_pUserInput = m_pUserInputFactory.getInstance();
}

void System::notifyStartup()
{
    m_ArduinoHal.getSerial().com_begin(DEBUGSERIAL_BAUDRATE);
    m_MessageHandler.printMessage("Startup");
    VoicePrompt startup;
    startup.promptId = MSG_STARTUP;
    startup.allowSkip = false;
    m_MessageHandler.promptMessage(startup);
}

void System::notifyShutdown()
{
    m_MessageHandler.printMessage("Shutdown");
    VoicePrompt shutdown;
    shutdown.promptId = MSG_SHUTDOWN;
    shutdown.allowSkip = false;
    m_MessageHandler.promptMessage(shutdown);
}

void System::shutdown()
{
    //m_MessageHandler.printMessage("Deleting objects");
    //m_PwrCtrl.allowShutdown();
}

void System::loop()
{
    m_DfMini.loop();
    /*
    UserInput_interface::eUserRequest userRequest{m_pUserInput->getxxx};
    NfcControl_interface::eTagState tagState{m_pNfcControl->getTagPresence()};

    // Handle Voice Menu
    m_pVoiceMenu->setTagState(tagState);
    m_pVoiceMenu->setUserInput(userRequest);
    m_pVoiceMenu->loop();

    // Handle Mp3 Playback
    m_pMp3Control->setTagState(tagState);
    m_pMp3Control->setUserInput(userRequest);
    m_pMp3Control->setBlocked(m_pVoiceMenu->isActive()); // VoiceMenu overrules Playback
    m_pMp3Control->loop();
    */
}

bool System::isShutdownRequested()
{
    //return (m_PwrCtrl.isShutdownRequested());
    return false;
}

void System::timer1Task_1ms()
{
    static volatile uint16_t ui16Ticks = 0;
    //m_pUserInput->userinputServiceIsr(); // userInput service 1ms task

    ++ui16Ticks;
    if (ui16Ticks >= MS_TO_S) // 1ms --> 1s
    {
        ui16Ticks = 0;
        timer1Task_1sec();
    }
}

void System::timer1Task_1sec()
{
    m_PwrCtrl.notifyTimerTick(); // idle timer and LED behavior
    m_LullabyeTimer.timerTick();
    m_MenuTimer.timerTick();
    m_DfMiniPromptTimer.timerTick();
}