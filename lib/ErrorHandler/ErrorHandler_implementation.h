#ifndef ERRORHANDLER_IMPLEMENTATION_H
#define ERRORHANDLER_IMPLEMENTATION_H

#include "Arduino_interface/Arduino_DIcontainer_interface.h"
#include "Mp3Control_interface/Mp3Control/Mp3Control_interface.h"

#include "ErrorHandler_interface.h"
#include "PlayError.h"


class ErrorHandler : public ErrorHandler_interface
{
public:
    ErrorHandler(Arduino_DIcontainer_interface *pArduHal,
                 NfcControl_interface *pNfcCtrl,
                 Mp3Control_interface *pMp3Ctrl, ) : m_pArduinoHal(pArduHal),
                                                           m_pNfcControl(pNfcCtrl),
                                                           m_pMp3Control(pMp3Ctrl){};

    void handleErrors() override;

    //#if DEBUGSERIAL
    void printDebugMessage() override;
    //#endif

    // Events
    void onStartup() override;
    void onShutdown() override;

    void setHelpRequested() override { m_playError.promptHelpRequested(); };
    void setTagReadError() override { m_playError.promptTagReadError(); };
    void setFolderError() override { m_playError.promptFolderError(); };

private:
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    NfcControl_interface *m_pNfcControl{nullptr};
    Mp3Control_interface *m_pMp3Control{nullptr};

    PlayError m_playError{PlayError(m_pMp3Control)};
};

#endif // ERRORHANDLER_IMPLEMENTATION_H