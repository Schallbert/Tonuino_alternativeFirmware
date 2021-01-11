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
    if (m_lastMessage[message.getGroup()] != message.getContents())
    {
        status = true;
        m_lastMessage[message.getGroup()] = message.getContents();
    }
    return status;
}

void MessageHandler::promptMessage(const VoicePrompt &prompt)
{
    if (isNewPrompt(prompt))
    {
        m_rDfMiniMp3.playMp3FolderTrack(prompt.getId());
        waitForPromptToStart();

        if (!prompt.getSkip())
        {
            waitForPromptToFinish();
        }
    }
}

bool MessageHandler::isNewPrompt(const VoicePrompt &prompt)
{
    bool result{false};
    if (prompt.getId() == 0)
    {
        return result;
    }

    if (m_lastPrompt != prompt)
    {
        m_lastPrompt = prompt;
        result = true;
    }
    return result;
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