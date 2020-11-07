#ifndef INPUTDISPATCHER_ERRORHANDER_H
#define INPUTDISPATCHER_ERRORHANDER_H

#include "../Nfc/Nfc_interface/Nfc_interface.h"
#include "../UserInput/UserInput_interface/UserInput_interface.h"

class InputDispatcher_ErrorHandler
{
    public:
    enum eDebugMessage
    {
        noMessage = 0,
        cardStateOutOfRange = 1,
        userInputOutOfRange = 2
    };

public:
    InputDispatcher_ErrorHandler(){};
    ~InputDispatcher_ErrorHandler(){};

public:
    void checkAndCorrectCardStateError(Nfc_interface::eTagState &tagState);
    void checkAndCorrectUserInputError(UserInput::eUserRequest &userRequest);

    const char *stringFromDebugMessage();

    private:
    eDebugMessage m_eDebugMessage{noMessage};
};

#endif // INPUTDISPATCHER_ERRORHANDER_H