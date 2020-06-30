    #ifndef MFRC522_IMPLEMENTATION_H
    #define MFRC522_IMPLEMENTATION_H

    #include <Arduino.h>
    #include "MFRC522_interface.h"
    #include "MFRC522.h"
    
    class Mfrc522 : public MFRC522_interface
    {
        public:
        void PCD_Init()
        {
            m_mfrc522.PCD_Init(); // Init MFRC522
        }
        void PCD_DumpVersionToSerial()
        {
            m_mfrc522.PCD_DumpVersionToSerial();
        }
        bool PICC_ReadCardSerial()
        {
            return m_mfrc522.PICC_ReadCardSerial();
        }
        bool PICC_IsNewCardPresent()
        {
            return m_mfrc522.PICC_IsNewCardPresent();
        }
        MFRC522::PICC_Type PICC_GetType(TagInfo *tag)
        {
            return 
        }
        MFRC522::StatusCode MIFARE_Write(byte blockAddr, 
                                         byte *buffer, 
                                         byte bufferSize)
        {
            return m_mfrc522.MIFARE_Write(
                                    blockaddr, 
                                    buffer, 
                                    bufferSize);
        }
        MFRC522::StatusCode MIFARE_Read(byte blockAddr, 
                                        byte *buffer, 
                                        byte *bufferSize)
        {
            return m_mfrc522.MIFARE_Read(
                                        blockAddr, 
                                        buffer, 
                                        bufferSize);
        }
        MFRC522::StatusCode PCD_Authenticate(byte command, 
                                             byte blockAddr,
                                             MIFARE_Key *key,
                                             Uid *uid
                                             )
        {
            return m_mfrc522.PCD_Authenticate(
                                            command, 
                                            blockAddr,
                                            key,
                                            uid
                                            );
        }
        MFRC522::StatusCode PCD_NTAG216_AUTH(byte* passWord, 
                                             byte pACK[]
                                             )
        {
            return m_mfrc522.PCD_NTAG216_AUTH(
                                            passWord, 
                                            pACK[]
                                            );
        }
        MFRC522::StatusCode PICC_HaltA()
        {
            return m_mfrc522.PICC_HaltA();
        }
        void PCD_StopCrypto1()
        {
            return m_mfrc522.PCD_StopCrypto1();
        }
        Uid get_uid()
        {
            return m_mfrc522.uid;
        }
        private:
        MFRC522 m_mfrc522{MFRC522(SS_PIN, RST_PIN)};
    };
    #endif // MFRC522_IMPLEMENTATION_H