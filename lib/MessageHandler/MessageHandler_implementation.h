#ifndef MESSAGEHANDLER_IMPLEMENTATION_H
#define MESSAGEHANDLER_IMPLEMENTATION_H

#include "Arduino_interface/Arduino_DIcontainer_interface.h"
#include "Mp3Play/Mp3Play_interface.h"

#include "MessageHandler_interface.h"
#include "PlayError.h"

class MessageHandler : public MessageHander_interface
{
public:
    MessageHandler(Arduino_DIcontainer_interface *pArduHal,
                   Mp3Play_interface *pMp3Play) : m_pArduinoHal(pArduHal),
                                                    m_pMp3Play(pMp3Play){};

    virtual void printMessage(const char *message) override;
    virtual void promptMessage(const VoicePrompt &message) override;

private:
        bool isNewValidPrompt(const VoicePrompt &message);
    bool isNewValidPrint(const char *message);
    bool isEqualPrint(const char *message);

private:
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    Mp3Play_interface *m_pMp3Play{nullptr};

    VoicePrompt m_lastPrompt;
    const char *m_lastPrint{nullptr};
};

#endif // MESSAGEHANDLER_IMPLEMENTATION_H