#include "MessageHandler_implementation.h"

void MessageHandler::printMessage(const char *message)
{
    if (isNewValidPrint(message))
    {
        m_pArduinoHal->getSerial()->com_println(m_lastPrint);
    }
}

bool MessageHandler::isNewValidPrint(const char *message)
{
    if (message == nullptr)
    {
        return false;
    }

    if (isEqualPrint(message))
    {
        return false;
    }

    m_lastPrint = message;
    return true;
}

bool MessageHandler::isEqualPrint(const char *message)
{
    bool result{true};
    while (*message && *m_lastPrint)
    {
        result &= (*message == *m_lastPrint);
        ++message;
        ++m_lastPrint;
    }
    return result;
}

void MessageHandler::promptMessage(const VoicePrompt &message)
{
    if (isNewValidPrompt(message))
    {
        m_pMp3Play->playPrompt(message);
    }
}

bool MessageHandler::isNewValidPrompt(const VoicePrompt &message)
{
    if (message.promptId == m_lastPrompt.promptId)
    {
        return false;
    }

    m_lastPrompt.promptId = message.promptId;
    m_lastPrompt.allowSkip = message.allowSkip;
    return true;
}