#ifndef USERINPUT_FACTORY_H
#define USERINPUT_FACTORY_H

#include "Arduino_config.h"
#include "ClickEncoder/ClickEncoder_implementation.h"
#include "UserInput_implementation.h"

#include "Mp3Prompt/Mp3Prompt_interface.h"

class UserInput_interface;
class UserInput_factory
{
public:
    explicit UserInput_factory(Mp3Prompt_interface &rPrompt,
                               MessageHander_interface &rMessageHandler) : m_rPrompt(rPrompt),
                                                                           m_rMessageHandler(rMessageHandler){

                                                                           };
    ~UserInput_factory() = default;
    UserInput_factory(const UserInput_factory &cpy) = delete;
    UserInput_factory &operator=(const UserInput_factory &cpy) = delete;

    // TODO: delete move constructors of mostly all classes

    UserInput_interface *getInstance()
    {
        if (m_pConcreteUserInput != nullptr)
        {
            delete m_pConcreteUserInput;
            m_pConcreteUserInput = nullptr;
        }

#if (USERINPUT_VARIANT == three_buttons)
        static Button_implementation m_pinPlPs(PINPLPS, USERINPUTACTIVE_STATE);
        static Button_implementation m_pinNext(PINNEXT, USERINPUTACTIVE_STATE);
        static Button_implementation m_pinPrev(PINPREV, USERINPUTACTIVE_STATE);
        m_pConcreteUserInput = new UserInput_3Buttons(
            m_pinPlPs,
            m_pinNext,
            m_pinPrev,
            m_rPrompt,
            m_rMessageHandler);
#elif (USERINPUT_VARIANT == one_encoder)
        m_pConcreteUserInput = new ClickEncoder_implementation(ENCA,
                                                               ENCB,
                                                               ENCSW,
                                                               rPrompt,
                                                               rMessageHandler,
                                                               ENC_STEPSPERNOTCH,
                                                               USERINPUTACTIVE_STATE);
#endif

        Message::eMessageContent message{Message::INPUTONLINE};
        if (m_pConcreteUserInput == nullptr)
        {
            message = Message::INPUTERROR;
        }
        m_rMessageHandler.printMessage(message);
        return m_pConcreteUserInput;
    }

private:
    Mp3Prompt_interface &m_rPrompt;
    MessageHander_interface &m_rMessageHandler;

    UserInput_interface *m_pConcreteUserInput{nullptr};
};

#endif // USERINPUT_FACTORY_h
