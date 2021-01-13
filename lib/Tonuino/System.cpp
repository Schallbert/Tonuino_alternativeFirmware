#include "System.h"

#include "../UserInput/UserInput/UserInput_implementation.h"
#include "../UserInput/ClickEncoder/ClickEncoder_implementation.h"

void System::init()
{
    // Timers must be initialized and started by now!
    m_ArduinoHal.getSerial().com_begin(DEBUGSERIAL_BAUDRATE);
    m_NfcControl.init();
    m_Mp3Play.init();
    m_PwrCtrl.requestKeepAlive();
    UserInput_factory m_pUserInputFactory{};
    m_pUserInput = m_pUserInputFactory.getInstance();
    notifyStartup();
}

void System::notifyStartup()
{
    m_MessageHandler.printMessage(Message{Message::STARTUP});
    VoicePrompt startup{VoicePrompt(VoicePrompt::MSG_STARTUP, false)};
    m_MessageHandler.promptMessage(startup);
}

void System::shutdown()
{
    notifyShutdown();
    m_PwrCtrl.allowShutdown();
}

void System::notifyShutdown()
{
    m_MessageHandler.printMessage(Message{Message::HALT});
    VoicePrompt shutdown{VoicePrompt(VoicePrompt::MSG_SHUTDOWN, false)};
    m_MessageHandler.promptMessage(shutdown);
}

void System::loop()
{
    m_DfMini.loop();

    UserInput_interface::eUserRequest userRequest{m_pUserInput->getUserRequest()};
    Message::eMessageContent tagState{m_NfcControl.getTagPresence()};

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
    if (m_pUserInput != nullptr)
    {
        m_pUserInput->userinputServiceIsr(); // userInput service 1ms task
    }
    m_PwrCtrl.service1msLed();

    ++ui16Ticks;
    if (ui16Ticks >= MS_TO_S) // 1ms --> 1s
    {
        ui16Ticks = 0;
        timer1Task_1sec();
    }
}

void System::timer1Task_1sec()
{
    m_PwrCtrl.notify1sTimer(); // idle timer and LED behavior
    m_LullabyeTimer.timerTick();
    m_MenuTimer.timerTick();
    m_DfMiniPromptTimer.timerTick();
}