#ifndef MESSAGEHANDLER_IMPLEMENTATION_H
#define MESSAGEHANDLER_IMPLEMENTATION_H

#include "Arduino_interface/Arduino_DIcontainer_interface.h"
#include "Mp3Play/Mp3Play_interface.h"

#include "MessageHandler_interface.h"

class MessageHandler : public MessageHander_interface
{
public:
    MessageHandler(Arduino_interface_com *pSerial,
                   Mp3Play_interface *pMp3Play) : m_pSerial(pSerial),
                                                  m_pMp3Play(pMp3Play){};

    virtual void printMessage(const char *message) override;
    virtual void promptMessage(const VoicePrompt &message) override;

private:
    bool isNewPrompt(const VoicePrompt &message);

private:
    Arduino_interface_com *m_pSerial{nullptr};
    Mp3Play_interface *m_pMp3Play{nullptr};

    VoicePrompt m_lastPrompt;
};

#endif // MESSAGEHANDLER_IMPLEMENTATION_H