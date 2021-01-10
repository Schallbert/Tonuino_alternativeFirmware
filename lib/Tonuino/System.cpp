#include "System.h"

#include "../UserInput/UserInput/UserInput_implementation.h"
#include "../UserInput/ClickEncoder/ClickEncoder_implementation.h"

void System::init()
{
    m_ArduinoHal.getSerial().com_begin(DEBUGSERIAL_BAUDRATE);
    m_NfcControl.init();
    m_Mp3Play.init();
    UserInput_factory m_pUserInputFactory{};
    m_pUserInput = m_pUserInputFactory.getInstance();
    notifyStartup();
}

void System::notifyStartup()
{
    m_MessageHandler.printMessage(Message{Message::STARTUP});
    VoicePrompt startup;
    startup.promptId = MSG_STARTUP;
    startup.allowSkip = false;
    m_MessageHandler.promptMessage(startup);
}

void System::shutdown()
{
    // TODO: shutdown should be delayed somehow? As otherwise timers cannot be stopped on time?!
    //notifyShutdown();
    //m_PwrCtrl.allowShutdown();
}

void System::notifyShutdown()
{
    m_MessageHandler.printMessage(Message{Message::HALT});
    VoicePrompt shutdown;
    shutdown.promptId = MSG_SHUTDOWN;
    shutdown.allowSkip = false;
    m_MessageHandler.promptMessage(shutdown);
}

void System::loop()
{
    m_DfMini.loop();

    UserInput_interface::eUserRequest userRequest{m_pUserInput->getUserRequest()};
    NfcControl_interface::eTagState tagState{m_NfcControl.getTagPresence()};

    // Handle Voice Menu
    /*
    m_VoiceMenu.setTagState(tagState);
    m_VoiceMenu.setUserInput(userRequest);
    m_VoiceMenu.loop();
    */

    // Handle Mp3 Playback
    m_Mp3Control.setTagState(tagState);
    m_Mp3Control.setUserInput(userRequest);
    m_Mp3Control.setBlocked(m_VoiceMenu.isActive()); // VoiceMenu overrules Playback
    m_Mp3Control.loop();
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