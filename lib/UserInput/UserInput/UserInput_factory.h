#ifndef USERINPUT_FACTORY_H
#define USERINPUT_FACTORY_H

#include "../ClickEncoder/ClickEncoder_implementation.h"

class UserInput_interface;
class UserInput_factory
{
public:
    enum eUserInputType
    {
        THREE_BUTTONS = 0,
        ONE_ENCODER = 1
    };

public:
    ~UserInput_factory();
    UserInput_interface *getInstance(eUserInputType input);

private:
    ClickEncoder_implementation *m_pinPlPs{nullptr};
    ClickEncoder_implementation *m_pinNext{nullptr};
    ClickEncoder_implementation *m_pinPrev{nullptr};
    ClickEncoder_implementation *m_enc{nullptr};

    UserInput_interface *m_instance{nullptr};
};

#endif // USERINPUT_FACTORY_h
