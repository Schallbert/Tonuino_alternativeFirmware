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
#if (USERINPUT_VARIANT == three_buttons)
    explicit UserInput_factory(Mp3Prompt_interface &rPrompt)
    {
        static ClickEncoder_implementation pinPlPs{ClickEncoder_implementation(PINPLPS, USERINPUTACTIVE_STATE)};
        static ClickEncoder_implementation pinNext{ClickEncoder_implementation(PINNEXT, USERINPUTACTIVE_STATE)};
        static ClickEncoder_implementation pinPrev{ClickEncoder_implementation(PINPREV, USERINPUTACTIVE_STATE)};
        static UserInput_3Buttons instance{UserInput_3Buttons(
            pinPlPs,
            pinNext,
            pinPrev,
            rPrompt,
            ENC_LONGPRESSREPEATINTERVAL)};
        m_instance = &instance;
    };
#elif (USERINPUT_VARIANT == one_encoder)
    explicit UserInput_factory(Mp3Prompt_interface &rPrompt)
    {
        static ClickEncoder_implementation m_enc{ClickEncoder_implementation(ENCA,
                                                                             ENCB,
                                                                             ENCSW,
                                                                             rPrompt,
                                                                             ENC_STEPSPERNOTCH,
                                                                             USERINPUTACTIVE_STATE)};
        static UserInput_ClickEncoder instance{UserInput_ClickEncoder(m_enc)};
        m_instance = &instance;
    };
#endif
    ~UserInput_factory() = default;
    UserInput_interface *getInstance() { return m_instance; };

private:
    UserInput_interface *m_instance;
};

#endif // USERINPUT_FACTORY_h
