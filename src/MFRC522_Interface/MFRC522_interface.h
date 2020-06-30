#ifndef MFRC522_INTERFACE_H
#define MFRC522_INTERFACE_H
#include <Arduino.h>
#include "MFRC522_minimal.h"

// Wrapper class to separate dependency MFRC522 card reader from utilizing classes.
// Enabler for mocking MFRC522 behavior.
class MFRC522_interface
{
    public:
    virtual ~MFRC522_interface() {};

    public:
    virtual void PCD_Init(void) = 0;
    virtual void PCD_DumpVersionToSerial(void) = 0;
    virtual bool PICC_ReadCardSerial(void) = 0;
    virtual bool PICC_IsNewCardPresent(void) = 0;
    virtual MFRC522::PICC_Type PICC_GetType(TagInfo *tag) = 0;
    virtual MFRC522::StatusCode MIFARE_Write(
                                             byte blockAddr, 
                                             byte *buffer,	
                                             byte bufferSize
                                             ) = 0;
	virtual MFRC522::StatusCode MIFARE_Read(
                                            byte blockAddr, 
                                            byte *buffer, 
                                            byte *bufferSize
                                            ) = 0;
    virtual MFRC522::StatusCode PCD_Authenticate(
                                                 byte command,
                                                 byte blockAddr,
                                                 MIFARE_Key *key,
                                                 Uid *uid
                                                 ) = 0;
    virtual MFRC522::StatusCode PCD_NTAG216_AUTH(byte* passWord, 
                                                 byte pACK[]
                                                 ) = 0;
    virtual MFRC522::StatusCode PICC_HaltA(void) = 0;
    virtual void PCD_StopCrypto1(void) = 0;
    virtual Uid get_uid(void) = 0; // returns the current chip's unique identifier
};
#endif // MFRC522_INTERFACE_H