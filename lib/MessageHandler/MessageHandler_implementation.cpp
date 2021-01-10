#include "Arduino_DIcontainer_interface.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"

#include "Tonuino_config.h"
#include "SimpleTimer/SimpleTimer.h"

#include "MessageHandler_implementation.h"
#include "../Arduino/messages.h"

void MessageHandler::printMessage(const Message &message)
{
    if (isNewMessage(message))
    {
        char *buffer = m_rMessages.getStringFromMessage(message);
        if (buffer != nullptr)
        {
            m_rSerial.com_println(buffer);
        }
    }
}

bool MessageHandler::isNewMessage(const Message &message)
{
    bool status{false};
    if (m_lastMessage[message.m_group] != message.m_contents)
    {
        status = true;
        m_lastMessage[message.m_group] = message.m_contents;
    }
    return status;
}

void MessageHandler::promptMessage(const VoicePrompt &message)
{
    if (isNewPrompt(message))
    {
        m_rDfMiniMp3.playMp3FolderTrack(message.promptId);
        waitForPromptToStart();

        if (!message.allowSkip)
        {
            waitForPromptToFinish();
        }
    }
}

bool MessageHandler::isNewPrompt(const VoicePrompt &message)
{
    bool result{(message.promptId != 0 &&
                 message.promptId != m_lastPrompt.promptId)};
    m_lastPrompt.promptId = message.promptId;
    m_lastPrompt.allowSkip = message.allowSkip;

    return (result);
}

void MessageHandler::waitForPromptToStart()
{
    m_rDfMiniPromptTimer.start(WAIT_DFMINI_READY);
    while (!m_rDfMiniMp3.isPlaying())
    {
        m_rDfMiniMp3.loop(); //wait for track to start (until timeout kicks in)
        if (m_rDfMiniPromptTimer.isElapsed())
        {
            Message timeout{Message(Message::PROMPTTIMEOUT)};
            printMessage(timeout);
            break;
        }
    }
    m_rDfMiniPromptTimer.stop();
}

void MessageHandler::waitForPromptToFinish()
{
    m_rDfMiniPromptTimer.start(TIMEOUT_PROMPT_PLAYED);
    while (m_rDfMiniMp3.isPlaying())
    {
        m_rDfMiniMp3.loop(); //wait for track to finish
        if (m_rDfMiniPromptTimer.isElapsed())
        {
            Message timeout{Message(Message::PROMPTTIMEOUT)};
            printMessage(timeout);
            break;
        }
    }
    m_rDfMiniPromptTimer.stop();
}