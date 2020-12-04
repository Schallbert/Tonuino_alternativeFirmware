#ifndef KEEPALIVE_H //include guard
#define KEEPALIVE_H

#include "../Arduino/Arduino_interface.h"

class KeepAlive
{
    /*  KeepAlive keeps the microcontroller and its periphery running
        until requested to shut down by the main program,
        e.g. after a timeout. As a KeepAlive function electronically
        can be designed in many sensible ways (TTL, Relay, Hi/Lo-Active etc.).
        Should be used like this:
        one button is electrically connected to the keepAlive circuitry,
        and paralelly - if wished - to an input of the microcontroller.
        When pressed, it will physically switch the system on.
        Upon system setup, keep_alive() is automatically called in the 
        class constructor. 
    */
public:
    //Note: For usage of a bistable relay, active must be inverted!
    KeepAlive(Arduino_interface_pins *pPinCtrl, uint8_t ui8PinID, bool bActiveState);

    // Physically keeps system powered (depending on external cirtuitry)
    void keep_alive();
    // Requests shut down to keep alive system; will be evaluated when "allow" method is called
    void request_shutdown();
    // returns if shutdown currently requested or not
    bool get_shutdown_request();
    // Will actually shut down once shutdown has been requested
    void allow_shutdown();

private:
    Arduino_interface_pins *m_pPinCtrl{nullptr};
    uint8_t m_ui8PinID;
    bool m_bPinActiveState{true};

    bool m_bShutDownRequested{false};
};
#endif //KEEPALIVE_H