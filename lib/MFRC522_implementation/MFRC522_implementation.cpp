#include "MFRC522_implementation.h"

void Mfrc522::initReader()
{
    m_pMfrc522.PCD_Init(); // Init MFRC522
}

bool Mfrc522::isCardPresent()
{
    return m_pMfrc522.PICC_ReadCardSerial();
}

bool Mfrc522::isNewCardPresent()
{
    return m_pMfrc522.PICC_IsNewCardPresent();
}

bool Mfrc522::writeCard(byte blockAddr, byte *dataToWrite)
{
    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    if (!setCardOnline())
    {
        m_eNotification = MFRC522_interface::errorTagSetOnlineFailed;
        return false;
    }

    if ((m_tagType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (m_tagType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (m_tagType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
        if (!authenticateMini1k4k())
        {
            setCardOffline();
            return false;
        }
        status = writeMini1k4k(blockAddr, dataToWrite);
    }
    else if (m_tagType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        if (!authenticateUltraLight())
        {
            setCardOffline();
            return false;
        }
        checkBlockAddressUltraLight(blockAddr);
        status = writeUltraLight(blockAddr, dataToWrite);
    }
    setCardOffline();

    if (status != MFRC522::STATUS_OK)
    {
        m_eNotification = MFRC522_interface::errorTagWrite;
        return false;
    }
    return true;
}

bool Mfrc522::readCard(byte blockAddr, byte *readResult)
{
    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    if (!setCardOnline())
    {
        return false;
    }

    if ((m_tagType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (m_tagType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (m_tagType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
        if (!authenticateMini1k4k())
        {
            setCardOffline();
            return false;
        }
        status = readMini1k4k(blockAddr, readResult);
    }
    else if (m_tagType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        if (!authenticateUltraLight())
        {
            setCardOffline();
            return false;
        }
        checkBlockAddressUltraLight(blockAddr);
        status = readUltraLight(blockAddr, readResult);
    }
    setCardOffline();

    if (status != MFRC522::STATUS_OK)
    {
        m_eNotification = MFRC522_interface::errorTagRead;
        return false;
    }
    return true;
}

MFRC522::StatusCode Mfrc522::writeMini1k4k(byte blockAddr, byte *data)
{
    return (MFRC522::StatusCode)m_pMfrc522.MIFARE_Write(blockAddr, data, NFCTAG_MEMORY_TO_OCCUPY);
}

MFRC522::StatusCode Mfrc522::writeUltraLight(byte blockAddr, byte *data)
{
    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    byte buffer[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY;

    for (uint8_t i = 0; i < MIFARE_UL_BLOCK_SIZE; ++i)
    {
        memset(buffer, 0, NFCTAG_MEMORY_TO_OCCUPY);
        // copy 4byte block from data to buffer
        memcpy(buffer, data + (i * MIFARE_UL_BLOCK_SIZE), MIFARE_UL_BLOCK_SIZE);
        status = (MFRC522::StatusCode)m_pMfrc522.MIFARE_Write(blockAddr + i, buffer, ui8_bufSize);
        if (status != MFRC522::STATUS_OK)
        {
            return status;
        }
    }
    return status;
}

MFRC522::StatusCode Mfrc522::readMini1k4k(byte blockAddr, byte *result)
{
    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY + 2; // Account for checksum
    byte buffer[ui8_bufSize];
    // NFC read procedure for certain types of Tag/Cards: Block of 18 bytes incl. checksum
    status = (MFRC522::StatusCode)m_pMfrc522.MIFARE_Read(blockAddr, buffer, &ui8_bufSize);
    memcpy(result, buffer, NFCTAG_MEMORY_TO_OCCUPY); // ignores checksum bytes
    return status;
}

MFRC522::StatusCode Mfrc522::readUltraLight(byte blockAddr, byte *result)
{
    MFRC522::StatusCode status = MFRC522::STATUS_OK;
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY + 2; // Account for checksum
    byte buffer[ui8_bufSize];

    for (uint8_t i = 0; i < MIFARE_UL_BLOCK_SIZE; ++i)
    {
        status = (MFRC522::StatusCode)m_pMfrc522.MIFARE_Read(blockAddr + 1, buffer, &ui8_bufSize);
        if (status != MFRC522::STATUS_OK)
        {
            return status;
        }
        // copy 4byte block from buffer2 to buffer
        memcpy(result + (i * MIFARE_UL_BLOCK_SIZE), buffer, MIFARE_UL_BLOCK_SIZE);
    }
    return status;
}

void Mfrc522::setCardOffline()
{
    m_pMfrc522.PICC_HaltA();
    m_pMfrc522.PCD_StopCrypto1();
    m_eNotification = MFRC522_interface::tagOffline;
}

bool Mfrc522::setCardOnline()
{
    // Try reading card
    if (m_pMfrc522.PICC_ReadCardSerial() != MFRC522::STATUS_OK)
    {
        m_eNotification = MFRC522_interface::errorTagSetOnlineFailed;
        setCardOffline();
        return false;
    }
    if (!getTagType())
    {
        m_eNotification = MFRC522_interface::errorTagAuthenticateFailed;
        return false;
    }
    return true;
}

bool Mfrc522::authenticateMini1k4k()
{
    MFRC522::StatusCode status;
    status = m_pMfrc522.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A,
        m_ui8TrailerBlockMini1k4k,
        &m_eKey,
        &(m_pMfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        m_eNotification = MFRC522_interface::errorTagAuthenticateFailed;
        return false;
    }
    return true;
}

bool Mfrc522::authenticateUltraLight()
{
    MFRC522::StatusCode status;
    byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag
    status = m_pMfrc522.PCD_NTAG216_AUTH(m_eKey.keyByte, pACK);
    if (status != MFRC522::STATUS_OK)
    {
        m_eNotification = MFRC522_interface::errorTagAuthenticateFailed;
        return false;
    }
    return true;
}

bool Mfrc522::getTagType()
{
    m_tagType = m_pMfrc522.PICC_GetType(m_pMfrc522.uid.sak);
    return (m_tagType != MFRC522::PICC_TYPE_UNKNOWN);
}

void Mfrc522::checkBlockAddressUltraLight(byte &blockAddress)
{
    // Make sure that blockAddr is within allowed range for MIFARE ultralight
    if (blockAddress < ULTRALIGHTSTARTPAGE)
    {
        blockAddress = ULTRALIGHTSTARTPAGE;
        m_eNotification = MFRC522_interface::errorTagRequestOutOfRange;
    }
    else if (blockAddress > ULTRALIGHTSTOPPAGE)
    {
        blockAddress = ULTRALIGHTSTOPPAGE;
        m_eNotification = MFRC522_interface::errorTagRequestOutOfRange;
    }
}
void Mfrc522::checkBlockAddressMini1k4k(byte &blockAddress)
{
    if (blockAddress == 0)
    {
        // 0 is reserved!
        blockAddress = 1;
        m_eNotification = MFRC522_interface::errorTagRequestOutOfRange;
    }
    if ((blockAddress % 4) == SECTORSTRAILERBLOCKMINI1K4K)
    {
        ++blockAddress;
        m_eNotification = MFRC522_interface::errorTagRequestOutOfRange;
    }
    // 4Blocks per sector
    m_ui8SectorMini1k4k = blockAddress / 4;
    // Block 3 of each sector is the trailer block containing keys
    // (init 0xFFFFFFFF), we won't touch those!
    m_ui8TrailerBlockMini1k4k = m_ui8SectorMini1k4k * 4 + 3;
}