#ifndef MFRC522_INTERFACE_H
#define MFRC522_INTERFACE_H
#include <Arduino.h>
#include <MFC522_minimal.h>
#include <MFC522_interface.h>

class Mock_MFRC522 : public MFRC522_interface
{
    public:
    // Method:  output    name   input   overrides functionality of interface
    MOCK_METHOD(void, PCD_Init, (void), (override));
    MOCK_METHOD(void, PCD_DumpVersionToSerial, (void), (override));
    MOCK_METHOD(bool, PICC_ReadCardSeriall, (void), (override));
    MOCK_METHOD(bool, PICC_IsNewCardPresent, (void), (override));
    MOCK_METHOD(MFRC522::PICC_Type, PICC_GetType, (TagInfo *tag), 
                                                    (override));
    MOCK_METHOD(MFRC522::StatusCode, MIFARE_Write, (byte blockAddr, 
                                                    byte *buffer,	
                                                    byte bufferSize), 
                                                    (override));
    MOCK_METHOD(MFRC522::StatusCode, MIFARE_Read, (byte blockAddr, 
                                                    byte *buffer,	
                                                    byte *bufferSize), 
                                                    (override));
    MOCK_METHOD(MFRC522::StatusCode, PCD_Authenticate, (byte command, 
                                                        byte blockAddr,	
                                                        MIFARE_Key *key,
                                                        Uid *uid), 
                                                        (override));
    MOCK_METHOD(MFRC522::StatusCode, PCD_NTAG216_AUTH, (byte* passWord, 
                                                        byte pACK[]),
                                                        (override));
    MOCK_METHOD(MFRC522::StatusCode, PICC_HaltA, (void), (override));
    MOCK_METHOD(void, PCD_StopCrypto1, (void), (override));
    MOCK_METHOD(Uid, PCD_StopCrypto1, (void), (override));
};
#endif // MFRC522_INTERFACE_H