#ifndef DELETEMENU_STATEMANAGER_h
#define DELETEMENU_STATEMANAGER_h

#include "../Arduino_HardwareAbstraction/Arduino_types.h"
#include "../Config/Tonuino_config.h"
#include "../Mp3/Mp3Prompt/VoicePrompt.h"


class DeleteMenu_StateManager
{
public:
    void confirm();
    void abort();

    void setTagToDeleteDetected();
    void setError();

    uint16_t getMenuStateMessage();

    bool isActive();
    bool isPendingConfirmDelete();
    bool isComplete();

private:
    void initMenu();
    void handleDeletionConfirmed();


private:
    enum eMenuState
    {
        NO_MENU = 0,
        PLACE_TAG_TO_DELETE = VoicePrompt::MSG_DELETETAG,
        CONFIRM_DELETE = VoicePrompt::MSG_CONFIRM_DELETION,
        COMPLETE = VoicePrompt::MSG_TAGCONFSUCCESS,
        ABORTED = VoicePrompt::MSG_ABORTED,
        ERROR = VoicePrompt::MSG_ERROR_CARDREAD
    };

    eMenuState menuState{NO_MENU};
    bool tagToDeleteDetected{false};
};

#endif // DELETEMENU_STATEMANAGER_h
