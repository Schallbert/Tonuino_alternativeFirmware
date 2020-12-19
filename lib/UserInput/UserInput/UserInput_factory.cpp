#include "Tonuino_config.h"
#include "Arduino_config.h"
#include "UserInput_implementation.h"

#include "UserInput_factory.h"

UserInput_factory::~UserInput_factory()
{
    if (m_pinPlPs != nullptr)
    {
        delete m_pinPlPs;
        delete m_pinPrev;
        delete m_pinNext;
    }

    if (m_enc != nullptr)
    {
        delete m_enc;
    }

    delete m_instance;
    m_instance = nullptr;
    m_enc = nullptr;
    m_pinPlPs = nullptr;
    m_pinPrev = nullptr;
    m_pinNext = nullptr;
}

UserInput_interface *UserInput_factory::getInstance(eUserInputType input)
{
    switch (input)
    {
    case THREE_BUTTONS:
        m_pinPlPs = new ClickEncoder_implementation(PINPLPS, USERINPUTACTIVE_STATE);
        m_pinNext = new ClickEncoder_implementation(PINPREV, USERINPUTACTIVE_STATE);
        m_pinPrev = new ClickEncoder_implementation(PINNEXT, USERINPUTACTIVE_STATE);
        m_instance = new UserInput_3Buttons(
            m_pinPlPs,
            m_pinNext,
            m_pinPrev,
            ENC_LONGPRESSREPEATINTERVAL);
        break;
    case ONE_ENCODER:
        m_enc = new ClickEncoder_implementation(ENCA,
                                                ENCB,
                                                ENCSW,
                                                ENC_STEPSPERNOTCH,
                                                USERINPUTACTIVE_STATE);
        m_instance = new UserInput_ClickEncoder(m_enc);
        break;
    default:
        m_instance = nullptr;
    }
    return m_instance;
}