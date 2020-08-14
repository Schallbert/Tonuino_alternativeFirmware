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

InputManager::eCardState InputManager::getCardState()
{
    bool cardPresent = m_pNfcTagReader->is_card_present();
    m_pUserInput->set_card_detected(cardPresent); // TODO: needed?

    if (cardPresent)
    {
        if (!m_pNfcTagReader->is_new_card_present())
        {
            return ACTIVE_KNOWN_CARD;
        }
        else // New card detected: runs once as new card is automatically set to ActiveCard
        {
            Folder dummyFolder;
            if (m_pNfcTagReader->read_folder_from_card(dummyFolder))
            {
                return NEW_KNOWN_CARD;
            }
            else // New card but folder cannot be read
            {
                if (!m_pNfcTagReader->is_known_card())
                {
                    return UNKNOWN_CARD_MENU;
                }
            }
        }
    }

    return NO_CARD;
}

UserInput::UserRequest_e InputManager::getUserInput()
{
    return m_pUserInput->get_user_request();
}
