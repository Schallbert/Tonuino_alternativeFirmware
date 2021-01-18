#ifndef USERINPUT_INTERFACE_H
#define USERINPUT_INTERFACE_H

class UserInput_interface
{
    //Interface class of UserInput implementation
    /*  UserInput gets the user's requests for device control,
        be it via buttons or click encoder(s), and relays the requests
        to the main program.
        This interface is set up to make the main program independent of
        the physical implementation of the user input.
    */
public:
    enum eUserRequest
    {
        NO_ACTION = 0,
        PLAY_PAUSE,
        PP_LONGPRESS,
        NEXT_TRACK,
        PREV_TRACK,
        INC_VOLUME,
        DEC_VOLUME,
        NUMBER_OF_REQUESTS = 7
    };

protected:
    bool userInputLocked{false};

public:
    virtual ~UserInput_interface(){};
    virtual eUserRequest getUserRequest(void) = 0; // returns user's request to main program
    virtual void userinputServiceIsr(void) = 0;  // recurring task to poll UserInput's connected hardware
}; // UserInput

#endif // USERINPUT_INTERFACE_H