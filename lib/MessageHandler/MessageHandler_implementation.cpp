#include "MessageHandler_implementation.h"

void MessageHandler::printMessage(const char *message)
{
    if (message != nullptr)
    {
        if(*message != 0)
        {
            m_pSerial->com_println(message);
        }     
    }
}

void MessageHandler::promptMessage(const VoicePrompt &message)
{
    if (isNewPrompt(message))
    {
        m_pMp3Play->playPrompt(message);
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