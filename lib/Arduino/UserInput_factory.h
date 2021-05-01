#ifndef USERINPUT_FACTORY_H
#define USERINPUT_FACTORY_H

#include "Mp3Prompt/Mp3Prompt_interface.h"
#include "MessageHandler_interface.h"

#include "UserInput/UserInput_interface.h"
#include "UserInput_implementation.h"

class UserInput_factory : public UserInput_interface
{
public:
    UserInput_factory() = default;
    ~UserInput_factory() = default;
    UserInput_factory(const UserInput_factory &cpy) = delete;

    Message::eMessageContent getUserRequest() override { return m_concreteInputVariant.getUserRequest(); }
    void userinputServiceIsr() override { m_concreteInputVariant.userinputServiceIsr(); }
    void init() override { m_concreteInputVariant.init(); }

private:
#if (USERINPUT_VARIANT == three_buttons)
    UserInput_3Buttons m_concreteInputVariant;
#elif (USERINPUT_VARIANT == one_encoder)
    UserInput_ClickEncoder m_concreteInputVariant;
#elif (USERINPUT_VARIANT == two_buttons)
    UserInput_2Buttons m_concreteInputVariant;
#endif
};

#endif // USERINPUT_FACTORY_h
