#ifndef MESSAGEHANDLER_IMPLEMENTATION_H
#define MESSAGEHANDLER_IMPLEMENTATION_H

#include "Arduino_DIcontainer_interface.h"
#include "SimpleTimer/SimpleTimer.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"

#include "MessageHandler_interface.h"

class MessageHandler : public MessageHander_interface
{
public:
    MessageHandler(Arduino_interface_com *pSerial,
                   SimpleTimer *pDfMiniPromptTimer,
                   DfMiniMp3_interface *pDfMini) : m_pSerial(pSerial),
                                                   m_pDfMiniPromptTimer(pDfMiniPromptTimer),
                                                   m_pDfMiniMp3(pDfMini){};

    virtual void printMessage(const char *message) override;
    virtual void promptMessage(const VoicePrompt &message) override;

private:
    bool isNewPrompt(const VoicePrompt &message);
    void waitForPromptToStart() const;
    void waitForPromptToFinish() const;

private:
    Arduino_interface_com *m_pSerial{nullptr};
    SimpleTimer *m_pDfMiniPromptTimer{nullptr};
    DfMiniMp3_interface *m_pDfMiniMp3{nullptr};

    VoicePrompt m_lastPrompt;
};

#endif // MESSAGEHANDLER_IMPLEMENTATION_H