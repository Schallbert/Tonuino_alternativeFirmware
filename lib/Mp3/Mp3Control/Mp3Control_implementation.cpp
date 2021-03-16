#include "Mp3Control/Mp3Control_implementation.h"

#include "Tonuino_config.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"
#include "../Folder/Folder.h"

void Mp3Control::setUserInput(Message::eMessageContent input)
{
    m_userInput = input;
}

void Mp3Control::playFolder(Folder &folder)
{
    m_rMp3Player.playFolder(folder);
    playIfPaused();
}

void Mp3Control::playIfPaused()
{
    if (!m_rDfMiniMp3.isPlaying())
    {
        play();
    }
}

void Mp3Control::loop()
{
    handlePromptStatus();
    handleUserInput();
    m_rPowerManager.setPlayback(m_rDfMiniMp3.isPlaying());
    m_rMp3Player.autoplay();
    m_rDfMiniMp3.printStatus();
}

void Mp3Control::handlePromptStatus()
{
    if (m_userInput != Message::INPUTNONE)
    {
        m_rMp3Prompt.stopPrompt();
    }
}

void Mp3Control::handleUserInput()
{
    handleLocked();

    // initialize array of function pointers to address state-event transitions
    // Order MUST be inline with INxxx of Message
    typedef Mp3Control PC;
    static const dispatcher dispatchTable[IN_REQUEST_OPTIONS] =
        {
            &PC::none, &PC::plPs, &PC::help, &PC::none, &PC::next, &PC::incV, &PC::prev, &PC::decV};
    dispatcher dispatchExecutor = dispatchTable[static_cast<uint8_t>(m_userInput) & 0x0F];
    (this->*dispatchExecutor)();
}

void Mp3Control::handleLocked()
{
    // Toggle lock status with doubleClick
    if ((m_userInput == Message::INPUTPLPSDC) && (m_tagState == Message::ACTIVETAG))
    {
        playIfPaused();
        if (m_isLocked)
        {
            m_isLocked = false;
            m_rMp3Prompt.playPrompt(VoicePrompt{VoicePrompt::MSG_BUTTONFREE,
                                                VoicePrompt::ANNOUNCEMENT});
        }
        else
        {
            m_isLocked = true;
            m_rMp3Prompt.playPrompt(VoicePrompt{VoicePrompt::MSG_BUTTONLOCK,
                                                VoicePrompt::ANNOUNCEMENT});
        }
    }

    if (m_isLocked)
    {
        m_userInput = Message::INPUTNONE;
    }
}

void Mp3Control::plPs()
{
    if (m_rDfMiniMp3.isPlaying())
    {
        pause();
    }
    else
    {
        play();
    }
}

void Mp3Control::play()
{
    m_rDfMiniMp3.start(); // Only successful if a track is entered.
    m_rMessageHandler.printMessage(Message::PLAY);
}

void Mp3Control::pause()
{
    m_rDfMiniMp3.pause();
    m_rMessageHandler.printMessage(Message::PAUSE);
}

void Mp3Control::next()
{
    m_rMp3Player.playNext();
    m_rMessageHandler.printMessage(Message::NEXT);
}

void Mp3Control::prev()
{
    m_rMp3Player.playPrev();
    m_rMessageHandler.printMessage(Message::PREV);
}

void Mp3Control::incV()
{
    if (m_rDfMiniMp3.getVolume() < VOLUME_MAX)
    {
        m_rDfMiniMp3.increaseVolume();
        m_rMessageHandler.printMessage(Message::VOLUP);
    }
}

void Mp3Control::decV()
{
    if (m_rDfMiniMp3.getVolume() > VOLUME_MIN)
    {
        m_rDfMiniMp3.decreaseVolume();
        m_rMessageHandler.printMessage(Message::VOLDN);
    }
}

void Mp3Control::help()
{
    VoicePrompt helpMessage(VoicePrompt::MSG_HELP, VoicePrompt::PROMPT_ALLOWSKIP);
    m_rMp3Prompt.playPrompt(helpMessage);
}