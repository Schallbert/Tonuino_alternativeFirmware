#include "InputDispatcher_ErrorHandler.h"


//#if DEBUGSERIAL
void InputDispatcher_ErrorHandler::checkAndCorrectCardStateError(Nfc_interface::eTagState &tagState)
{
    m_eDebugMessage = noMessage;
    if ((tagState < Nfc_interface::NO_TAG) ||
        (tagState >= Nfc_interface::NUMBER_OF_TAG_STATES))
    {
        m_eDebugMessage = cardStateOutOfRange;
        tagState = Nfc_interface::NO_TAG;
    }
}

void InputDispatcher_ErrorHandler::checkAndCorrectUserInputError(UserInput::eUserRequest &userRequest)
{
    m_eDebugMessage = noMessage;
    if ((userRequest < UserInput::NO_ACTION) ||
        (userRequest >= UserInput::NUMBER_OF_REQUESTS))
    {
        m_eDebugMessage = cardStateOutOfRange;
        userRequest = UserInput::NO_ACTION;
    }
}

const char *InputDispatcher_ErrorHandler::stringFromDebugMessage()
{
    static const char *NOTIFY_STRING[] = {
        "No Message",
        "cardState out of range",
        "userInput out of range",
    };

    return NOTIFY_STRING[m_eDebugMessage];
};
//#endif
