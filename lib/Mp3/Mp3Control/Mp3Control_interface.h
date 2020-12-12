#ifndef MP3CONTROL_INTERFACE_H
#define MP3CONTROL_INTERFACE_H

#include "../Arduino/Arduino_types.h"
#include "../UserInput/UserInput/UserInput_interface.h"
#include "../Nfc/Nfc/Nfc_interface.h"

// API to control the MP3 player hardware
class Mp3Control_interface
{
public:
    virtual ~Mp3Control_interface(){};

public:
    virtual void setUserInput(UserInput_interface::eUserRequest input) = 0;
    virtual void setTagState(NfcControl_interface::eTagState input) = 0;
    virtual void setBlocked(bool isBlocked) = 0;
    // Call in main loop. Listens for DFminiMp3 replies & autoplays next song
    virtual void loop() = 0;
};

#endif // MP3CONTROL_INTERFACE_H