#include "NfcTag_implementation_MifareMini1k4k.h"

bool NfcTag_implementation_MifareMini1k4k::readTag(byte blockAddr, byte *readResult)
{
}

bool NfcTag_implementation_MifareMini1k4k::writeTag(byte blockAddr, byte *dataToWrite)
{
}

void NfcTag_implementation_MifareMini1k4k::checkAndRectifyBlockAddress(byte &blockAddress)
{
    if (blockAddress == 0)
    {
        // 0 is reserved!
        blockAddress = 1;
        m_eNotification = errorTagRequestOutOfRange;
    }
    if ((blockAddress % 4) == SECTORSTRAILERBLOCKMINI1K4K)
    {
        ++blockAddress;
        m_eNotification = errorTagRequestOutOfRange;
    }
    // 4Blocks per sector
    m_ui8SectorMini1k4k = blockAddress / 4;
    // Block 3 of each sector is the trailer block containing keys
    // (init 0xFFFFFFFF), we won't touch those!
    m_ui8TrailerBlockMini1k4k = m_ui8SectorMini1k4k * 4 + 3;
}