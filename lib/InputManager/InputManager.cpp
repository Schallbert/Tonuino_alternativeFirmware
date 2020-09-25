#include "InputManager.h"

uint32_t InputManager::getRandomSeed()
{
    uint32_t ADC_LSB;
    uint32_t ADCSeed;
    for (uint8_t i = 0; i < 128; i++)
    {
        ADC_LSB = m_pPinControl->analog_read(PINANALOG_RNDMGEN) & 0x1;
        ADCSeed ^= ADC_LSB << (i % 32);
    }
    return ADCSeed; // Init Arduino random generator
}

UserInput::UserRequest_e InputManager::getUserInput()
{
    return m_pUserInput->get_user_request();
}
