#ifndef MFRC522_INTERFACE_H
#define MFRC522_INTERFACE_H

#include "Arduino_types.h"

class MFRC522_interface
{
    public:
    enum PICC_Type : byte {
		PICC_TYPE_UNKNOWN		,
		PICC_TYPE_ISO_14443_4	,	// PICC compliant with ISO/IEC 14443-4 
		PICC_TYPE_ISO_18092		, 	// PICC compliant with ISO/IEC 18092 (NFC)
		PICC_TYPE_MIFARE_MINI	,	// MIFARE Classic protocol, 320 bytes
		PICC_TYPE_MIFARE_1K		,	// MIFARE Classic protocol, 1KB
		PICC_TYPE_MIFARE_4K		,	// MIFARE Classic protocol, 4KB
		PICC_TYPE_MIFARE_UL		,	// MIFARE Ultralight or Ultralight C
		PICC_TYPE_MIFARE_PLUS	,	// MIFARE Plus
		PICC_TYPE_MIFARE_DESFIRE,	// MIFARE DESFire
		PICC_TYPE_TNP3XXX		,	// Only mentioned in NXP AN 10833 MIFARE Type Identification Procedure
		PICC_TYPE_NOT_COMPLETE	= 0xff	// SAK indicates UID is not complete.
	};
    
    public:
    virtual ~MFRC522_interface(){};

    public:
    virtual void init() = 0;
    virtual void softPowerDown() = 0;
	virtual void softPowerUp() = 0;
    virtual void tagHalt() = 0;
    virtual void tagLogoff() = 0;
    virtual PICC_Type getTagType() = 0;

    virtual bool tagRead(byte blockAddr, byte *buffer, byte *bufferSize) = 0;
    virtual bool tagWrite(byte blockAddr, byte *buffer, byte bufferSize) = 0;

    virtual bool isNewCardPresent() = 0;
	virtual bool readCardSerial() = 0;
};

#endif // MFRC522_INTERFACE_H