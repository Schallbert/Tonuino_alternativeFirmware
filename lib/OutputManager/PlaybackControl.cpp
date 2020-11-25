#include "PlaybackControl.h"

void PlaybackControl::handleUserInput(UserInput::eUserRequest input)
{
    // initialize array of function pointers to address state-event transitions
    typedef PlaybackControl PC;
    static const dispatcher dispatchTable[UserInput::NUMBER_OF_REQUESTS] =
        {
            //NOAC,     PL_PS,     PP_LP,     NEXT_,     PREV_,     INC_V,     DEC_V,
            &PC::none, &PC::plPs, &PC::help, &PC::next, &PC::prev, &PC::incV, &PC::decV // NO_TAG / ACTIVE_KNOWN_TAG
        };
    dispatcher dispatchExecutor = dispatchTable[input];
    (this->*dispatchExecutor)();
}