#ifndef CLICKENCODER_IMPLEMENTATION_H
#define CLICKENCODER_IMPLEMENTATION_H

#include "ClickEncoder_Abstraction/ClickEncoder_interface.h"
#include "ClickEncoder.h"

class ClickEncoder_implementation : public ClickEncoder_interface
{
public:
    // full ClickEncoder
    ClickEncoder_implementation(uint8_t pinA,
                                uint8_t pinB,
                                uint8_t pinButton,
                                uint8_t stepsPerNotch,
                                bool buttonActiveState) : m_Encoder(pinA,
                                                                    pinB,
                                                                    pinButton,
                                                                    stepsPerNotch,
                                                                    buttonActiveState){};
    ~ClickEncoder_implementation() = default;
    ClickEncoder_implementation(const ClickEncoder_implementation &copySrc) = delete;

public:
    void service() override
    {
        m_Encoder.service();
    }

    int16_t getIncrement() override
    {
        return m_Encoder.getIncrement();
    }

    eButtonState getButton() override
    {
        return static_cast<eButtonState>(m_Encoder.getButton());
    }

    void setAccelerationEnabled(const bool enabled) //override
    {
        m_Encoder.setAccelerationEnabled(enabled);
    }

    void setDoubleClickEnabled(const bool enabled) //override
    {
        m_Encoder.setDoubleClickEnabled(enabled);
    }

    void setLongPressRepeatEnabled(const bool enabled) // override
    {
        m_Encoder.setLongPressRepeatEnabled(enabled);
    }

private:
    ClickEncoder m_Encoder;
};

class Button_implementation : public ClickEncoder_interface
{
public:
    Button_implementation(uint8_t pinButton,
                          bool buttonActiveState) : m_Button(pinButton,
                                                             buttonActiveState){};
    ~Button_implementation() = default;
    Button_implementation(const Button_implementation &copySrc) = delete;

    void service() override
    {
        m_Button.service();
    }

    eButtonState getButton() override
    {
        return static_cast<eButtonState>(m_Button.getButton());
    }

    void setDoubleClickEnabled(const bool enabled) override
    {
        m_Button.setDoubleClickEnabled(enabled);
    }

    void setLongPressRepeatEnabled(const bool enabled) override
    {
        m_Button.setLongPressRepeatEnabled(enabled);
    }

    int16_t getIncrement() override  { return 0; }; // no behavior   
    void setAccelerationEnabled(const bool enabled) override { return; }; // no behavior

private:
    Button m_Button;
};

#endif // CLICKENCODER_IMPLEMENTATION_H