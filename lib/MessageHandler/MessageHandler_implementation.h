#ifndef ERRORHANDLER_IMPLEMENTATION_H
#define ERRORHANDLER_IMPLEMENTATION_H

#include "Arduino_interface/Arduino_DIcontainer_interface.h"
#include "Mp3Control_interface/Mp3Control/Mp3Control_interface.h"

#include "MessageHandler_interface.h"
#include "PlayError.h"

class MessageHandler : public MessageHander_interface
{
public:
    MessageHandler(Arduino_DIcontainer_interface *pArduHal,
                 NfcControl_interface *pNfcCtrl,
                 Mp3Control_interface *pMp3Ctrl, ) : m_pArduinoHal(pArduHal),
                                                     m_pNfcControl(pNfcCtrl),
                                                     m_pMp3Control(pMp3Ctrl){};

    virtual void handleErrors() = 0;

    // Events
    void onStartup() override;
    void onShutdown() override;

    virtual void printMessage(char* message); // TODO: implement
    virtual void setMessagePrompt(uint16_t messageCode)
    {
        m_error.promptId = messageCode;
        m_error.allowSkip = false;
    };

private:
    void playErrorMessage();
    void printDebugMessage();

private:
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    NfcControl_interface *m_pNfcControl{nullptr};
    Mp3Control_interface *m_pMp3Control{nullptr};

    PlayError m_playError{PlayError(m_pMp3Control)};

    VoicePrompt m_error{};
};

#endif // ERRORHANDLER_IMPLEMENTATION_H