#ifndef MFRC522_IMPLEMENTATION_H
#define MFRC522_IMPLEMENTATION_H

#include "MFRC522_interface.h
#include "MFRC522.h"

// Wrapper class for actual MFRC522 Library implementation.
// Introduced for clarity and testability.
class MFRC522_implementation : public MFRC522_interface
{

    public:
    void init() { m_Mfrc522.PCD_Init(); };
    void softPowerDown() { m_Mfrc522.PCD_SoftPowerDown(); };
	void softPowerUp() { m_Mfrc522.PCD_SoftPowerUp(); };
    void tagHalt() { m_Mfrc522.PICC_HaltA(); };
    void tagLogoff() { m_Mfrc522.PCD_StopCrypto1(); };
    PICC_Type getTagType() { return m_pMfrc522.PICC_GetType(m_pMfrc522.uid.sak); }; // most likely cast needed

    bool tagRead(byte blockAddr, byte *buffer, byte *bufferSize) 
    { return (m_Mfrc522.MIFARE_Read(blockAddr, buffer, bufferSize) == MFRC522::STATUS_OK); };
    bool tagWrite(byte blockAddr, byte *buffer, byte bufferSize)
    { return (m_Mfrc522.MIFARE_Write(blockAddr, buffer, bufferSize) == MFRC522::STATUS_OK); };

    bool isNewCardPresent() { return m_Mfrc522.PICC_IsNewCardPresent(); };
	bool isCardPresent() { return m_Mfrc522.PICC_ReadCardSerial(); };

private:
    MFRC522 m_Mfrc522{MFRC522(SS_PIN, RST_PIN)};
};

#endif //MFRC522_IMPLEMENTATION_H