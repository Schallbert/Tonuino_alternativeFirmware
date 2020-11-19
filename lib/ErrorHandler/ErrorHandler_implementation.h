#ifndef ERRORHANDLER_IMPLEMENTATION_H
#define ERRORHANDLER_IMPLEMENTATION_H

#include "ErrorHandler_interface.h"
#include "Arduino_interface/Arduino_DIcontainer_interface.h"
#include "Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"

class ErrorHandler : public ErrorHandler_interface
{
public:
    ErrorHandler(Arduino_DIcontainer_interface *pArduHal,
                 Mp3PlayerControl_interface *pMp3Ctrl) : m_pArduinoHal(pArduHal),
                                                         m_pMp3Control(pMp3Ctrl){};

    void handleErrors() override;

    //#if DEBUGSERIAL
    void printDebugMessage() override;
    //#endif

    // Events
    void onStartup() override;
    void onShutdown() override;

    void setHelpRequested() override;
    void setTagReadError() override;
    void setFolderError() override;

private:
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    Mp3PlayerControl_interface *m_pMp3Control{nullptr};

    
};

#endif // ERRORHANDLER_IMPLEMENTATION_H