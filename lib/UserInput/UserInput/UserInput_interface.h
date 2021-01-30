#ifndef USERINPUT_INTERFACE_H
#define USERINPUT_INTERFACE_H

#include "Messages_interface.h"

class UserInput_interface
{
    //Interface class of UserInput implementation
    /*  UserInput gets the user's requests for device control,
        be it via buttons or click encoder(s), and relays the requests
        to the main program.
        This interface is set up to make the main program independent of
        the physical implementation of the user input.
    */

protected:
    bool userInputLocked{false};

public:
    virtual ~UserInput_interface(){};
    virtual Message::eMessageContent getUserRequest() = 0; // returns user's request to main program
    virtual void userinputServiceIsr() = 0;  // recurring task to poll UserInput's connected hardware
}; // UserInput

#endif // USERINPUT_INTERFACE_H