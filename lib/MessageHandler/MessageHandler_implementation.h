#ifndef MESSAGEHANDLER_IMPLEMENTATION_H
#define MESSAGEHANDLER_IMPLEMENTATION_H

//#include "Arduino_DIcontainer_interface.h"
//#include "DfMiniMp3/DFMiniMp3_interface.h"

#include "MessageHandler_interface.h"

class Arduino_interface_com;
class DfMiniMp3_interface;
class SimpleTimer;

class MessageHandler : public MessageHander_interface
{
public:
    MessageHandler(Arduino_interface_com &rSerial,
                   DfMiniMp3_interface &rDfMini,
                   SimpleTimer &rDfMiniPromptTimer);

    virtual void printMessage(const char *message) override;
    virtual void promptMessage(const VoicePrompt &message) override;

private:
    bool isNewPrompt(const VoicePrompt &message);
    void waitForPromptToStart();
    void waitForPromptToFinish();

private:
    Arduino_interface_com &m_rSerial;
    DfMiniMp3_interface &m_rDfMiniMp3;
    SimpleTimer &m_rDfMiniPromptTimer;

    VoicePrompt m_lastPrompt;
};

#endif // MESSAGEHANDLER_IMPLEMENTATION_H