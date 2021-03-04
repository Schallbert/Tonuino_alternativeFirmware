#ifndef MFRC522_IMPLEMENTATION_H
#define MFRC522_IMPLEMENTATION_H

#include "../Nfc/MFRC522/MFRC522_interface.h"
#include "MFRC522.h"

// Wrapper class for actual MFRC522 Library implementation.
// Introduced for clarity and testability.
class MFRC522_implementation : public MFRC522_interface
{
public:
    MFRC522_implementation() = default;
    ~MFRC522_implementation() = default;
    MFRC522_implementation(const MFRC522_implementation &cpy) = delete;

public:
    void init() override
    {
        SPI.begin();
        m_Mfrc522.PCD_Init();
        m_Mfrc522.PCD_DumpVersionToSerial();
    }

    void softPowerDown() override { m_Mfrc522.PCD_SoftPowerDown(); }
    void softPowerUp() override { m_Mfrc522.PCD_SoftPowerUp(); }
    bool getTagUid() override { return m_Mfrc522.PICC_ReadCardSerial(); }
    bool tagLogin(byte blockAddress) override
    {
        return (m_Mfrc522.PCD_Authenticate(m_command, blockAddress, &m_key, &m_Mfrc522.uid) == MFRC522::STATUS_OK);
    }
    void tagHalt() override { m_Mfrc522.PICC_HaltA(); }
    void tagLogoff() override { m_Mfrc522.PCD_StopCrypto1(); }
    eTagType getTagType() override { return static_cast<eTagType>(m_Mfrc522.PICC_GetType(m_Mfrc522.uid.sak)); }; // most likely cast needed

    bool tagRead(byte blockAddress, byte *buffer, byte bufferSize) override
    {
        return (m_Mfrc522.MIFARE_Read(blockAddress, buffer, &bufferSize) == MFRC522::STATUS_OK);
    }

    bool tagWrite(byte blockAddress, byte *buffer, byte bufferSize) override
    {
        return (m_Mfrc522.MIFARE_Write(blockAddress, buffer, bufferSize) == MFRC522::STATUS_OK);
    }

    bool setTagActive() override 
    {
        byte bufferATQA[2];
        byte bufferSize = sizeof(bufferATQA);

        MFRC522::StatusCode result = m_Mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
        return (result == MFRC522::STATUS_OK || result == MFRC522::STATUS_COLLISION);
    }

    bool isTagPresent() override
    {
        bool result{setTagActive()};
        m_Mfrc522.PICC_HaltA(); // set to "sleep"
        return result;
    }

private:
    MFRC522 m_Mfrc522{SS_PIN, RST_PIN};
    // Key and auth commands hardcoded to streamline interface
    MFRC522::MIFARE_Key m_key = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}; // 6 byte key, factory default all set.
    MFRC522::PICC_Command m_command{MFRC522::PICC_CMD_MF_AUTH_KEY_A};   // authentication command, hardcoded to Key A
};

#endif //MFRC522_IMPLEMENTATION_H