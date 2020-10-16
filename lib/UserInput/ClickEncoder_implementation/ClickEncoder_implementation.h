#ifndef CLICKENCODER_IMPLEMENTATION_H
#define CLICKENCODER_IMPLEMENTATION_H

#include "../UserInput/ClickEncoder_interface/ClickEncoder_interface.h"
#include "ClickEncoder.h"

class ClickEncoder_implementation : public ClickEncoder_interface
{
public:
    ClickEncoder_implementation(uint8_t pinA, uint8_t pinB, uint8_t pinButton, uint8_t stepsPerNotch, bool buttonActiveState)
    {
        m_pEncoder = new ClickEncoder(pinA, pinB, pinButton, stepsPerNotch, buttonActiveState);
    };
    ~ClickEncoder_implementation()
    {
        delete m_pEncoder();
    };

public:
    void service() override
    {
        m_pEncoder->service();
    }

    int16_t getValue() override
    {
        return m_pEncoder->getValue();
    }
   
    eButtonState getButton() override
    {
        return static_cast<eButtonState>(m_pEncoder->getButton());
    }
     
    void setAccelerationEnabled(const bool &enabled) override
    {
        m_pEncoder->setAccelerationEnabled(enabled);
    }

    void void setDoubleclickEnabled(const bool &enabled) override
    {
        m_pEncoder->setDoubleclickEnabled(enabled);
    }


private:
    ClickEncoder *m_pEncoder{nullptr};
}

#endif // CLICKENCODER_IMPLEMENTATION_H