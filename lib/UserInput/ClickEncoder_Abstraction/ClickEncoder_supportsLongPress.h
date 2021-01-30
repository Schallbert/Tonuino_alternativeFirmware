#ifndef CLICKENCODER_SUPPORTSLONGPRESS_H
#define CLICKENCODER_SUPPORTSLONGPRESS_H

#include "ClickEncoder_interface.h"

/* This class enhances ClickEncoder by a longPressRepeat detection
    LongPressRepeat will periodically be set to enable button quick increment behavior
*/
class Encoder_longPressRepeat
{
public:
    Encoder_longPressRepeat(ClickEncoder_interface &rEncoder,
                            const uint16_t &ui16longPressRepeatInterval) : m_rEncoder(rEncoder),
                                                                           m_ui16longPressRepeatInterval(ui16longPressRepeatInterval){};
    ~Encoder_longPressRepeat() = default;

public:
    void service(void)
    {
        m_rEncoder.service();
        if (m_rEncoder.getButton() == ClickEncoder_interface::Held)
        {
            ++m_ui16ButtonHeldTicks;
        }
        else
        {
            m_ui16ButtonHeldTicks = 0;
        }
    }

    ClickEncoder_interface::eButtonState getButton()
    {
        ClickEncoder_interface::eButtonState buttonState{ClickEncoder_interface::Open};
        buttonState = m_rEncoder.getButton();
        if (buttonState == ClickEncoder_interface::Held &&
            m_ui16longPressRepeatInterval > 0)
        {
            if (m_ui16ButtonHeldTicks > m_ui16longPressRepeatInterval)
            {
                buttonState = ClickEncoder_interface::LongPressRepeat;
                m_ui16ButtonHeldTicks = 0;
            }
        }
        return buttonState;
    }

    int16_t getValue()
    {
        return m_rEncoder.getValue();
    }

    void setAccelerationEnabled(const bool &enabled)
    {
        m_rEncoder.setAccelerationEnabled(enabled);
    }

    void setDoubleClickEnabled(const bool &enabled)
    {
        m_rEncoder.setDoubleClickEnabled(enabled);
    }

private:
    ClickEncoder_interface &m_rEncoder;
    const uint16_t &m_ui16longPressRepeatInterval; // tick level to detect long press repeat
    volatile uint16_t m_ui16ButtonHeldTicks{0};    // tick counter for long press and repeat long press detection
};

#endif // CLICKENCODER_SUPPORTSLONGPRESS_H