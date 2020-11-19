#ifndef ERRORHANDLER_INTERFACE_H
#define ERRORHANDLER_INTERFACE_H

// Requires HAL to print to serial
// Requires PromptManager to prompt error/help messages.
class ErrorHandler_interface
{

public:
    virtual ~ErrorHandler_interface(){};

    // Handler
    virtual void handleErrors() = 0;

    //#if DEBUGSERIAL
    virtual void printDebugMessage() = 0;
    //#endif

    // Events
    virtual void onStartup() = 0;
    virtual void onShutdown() = 0;


    virtual void setHelpRequested() = 0;
    virtual void setTagReadError() = 0;
    virtual void setFolderError() = 0;
};

#endif // ERRORHANDLER_INTERFACE_H
