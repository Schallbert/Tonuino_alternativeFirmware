#ifndef CLICKENCODER_IMPLEMENTATION_H
#define CLICKENCODER_IMPLEMENTATION_H

#include "ClickEncoder_Abstraction/ClickEncoder_interface.h"
#include "ClickEncoder.h"

class ClickEncoder_implementation : public ClickEncoder_interface
{
public:
    // button only
    ClickEncoder_implementation(uint8_t pinButton,
                                bool buttonActiveState) : m_rEncoder(ClickEncoder(-1, -1, pinButton, 4, buttonActiveState)){};

    // full ClickEncoder
    ClickEncoder_implementation(uint8_t pinA,
                                uint8_t pinB,
                                uint8_t pinButton,
                                uint8_t stepsPerNotch,
                                bool buttonActiveState) : m_rEncoder(ClickEncoder(pinA,
                                                                                  pinB,
                                                                                  pinButton,
                                                                                  stepsPerNotch,
                                                                                  buttonActiveState)){};

public:
    void service() override
    {
        m_rEncoder.service();
    }

    int16_t getValue() override
    {
        return m_rEncoder.getValue();
    }

    eButtonState getButton() override
    {
        return static_cast<eButtonState>(m_rEncoder.getButton());
    }

    void setAccelerationEnabled(const bool &enabled) override
    {
        m_rEncoder.setAccelerationEnabled(enabled);
    }

    void setDoubleClickEnabled(const bool &enabled) override
    {
        m_rEncoder.setDoubleClickEnabled(enabled);
    }

private:
    ClickEncoder m_rEncoder;
};

#endif // CLICKENCODER_IMPLEMENTATION_H