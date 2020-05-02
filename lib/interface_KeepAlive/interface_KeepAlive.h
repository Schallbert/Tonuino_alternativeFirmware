#ifndef KEEPALIVE_H //include guard
    #define KEEPALIVE_H

#ifndef ARDUINO_H
    #include "Arduino.h"
#endif

class KeepAlive
{
    /*  KeepAlive keeps the microcontroller and its periphery running
        until requested to shut down by the main program,
        e.g. after a timeout. As a KeepAlive function electronically
        can be designed in many sensible ways (TTL, Relay, Hi/Lo-Active etc.)
        this interface definition is kept abstract.
    */
    public: 
        //Note: For usage of a bistable relay, active must be inverted!
        KeepAlive(uint8_t pinID, bool active);
        
        void keep_alive();
        void shut_down();
        
    private:
        uint8_t pinID;
        bool active;
};
#endif //KEEPALIVE_H