#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Tonuino_config.h"

#include "mocks/unittest_Nfc_mocks.h"

#include "../Nfc/NfcTag/NfcTag_factory.h"

// This file tests the Nfc implementation
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class NfcTag_getInstance : public ::testing::Test
{
protected:
    NiceMock<Mock_MFRC522> m_Mfrc;
};

class NfcTag_readTag : public NfcTag_getInstance
{
};
class NfcTag_writeTag : public NfcTag_getInstance
{
    virtual void SetUp()
    {
        NfcTag_getInstance::SetUp();
        // get around constness
        for (uint8_t i = 0; i < 16; ++i)
        {
            writeBuffer[i] = fakeBufferData[i];
        }
    }

protected:
    byte writeBuffer[NFCTAG_MEMORY_TO_OCCUPY] = {};
};

TEST_F(NfcTag_getInstance, TagTypeUNKNOWN_returnsNullptr)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_UNKNOWN));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_Mfrc));
}

TEST_F(NfcTag_getInstance, TagTypeISO14443_returnsNullptr)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_ISO_14443_4));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_Mfrc));
}

TEST_F(NfcTag_getInstance, TagTypeISO18092_returnsNullptr)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_ISO_18092));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_Mfrc));
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREMINI_returnsInstance)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_MINI));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    EXPECT_CALL(m_Mfrc, tagLogin(_)).WillOnce(Return(false));
    byte *buffer{nullptr};
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFARE1K_returnsInstance)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    EXPECT_CALL(m_Mfrc, tagLogin(_)).WillOnce(Return(false));
    byte *buffer{nullptr};
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFARE4K_returnsInstance)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_4K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    EXPECT_CALL(m_Mfrc, tagLogin(_)).WillOnce(Return(false));
    byte *buffer{nullptr};
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREUL_returnsInstance)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    EXPECT_CALL(m_Mfrc, tagLogin(_)).WillOnce(Return(false));
    byte *buffer{nullptr};
    nfcInstance->readTag(4, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREUL_returnsNullptr)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_PLUS));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_Mfrc));
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREDESFIRE_returnsNullptr)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_DESFIRE));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_Mfrc));
}

TEST_F(NfcTag_getInstance, TagTypeTNP3XXX_returnsNullptr)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_TNP3XXX));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_Mfrc));
}

TEST_F(NfcTag_getInstance, TagTypeINCOMPLETE_returnsNullptr)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_NOT_COMPLETE));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_Mfrc));
}

TEST_F(NfcTag_readTag, MifareMini1k4k_setOnlineFails_returnsFalse)
{
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(false));
    EXPECT_CALL(m_Mfrc, tagRead(_, _, _)).Times(0);
    ASSERT_EQ(false, nfcInstance->readTag(1, buffer));
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tagReadCalledWithCorrectArguments)
{
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagRead(1, _, NFCTAG_MEMORY_TO_OCCUPY + 2));
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_readTag, MifareMini1k4k_wrongBlockAddress_automaticallyCorrected)
{
    uint8_t wrongBlockAddress = 0;
    uint8_t automaticallyCorrectedBlockAddress = 1;
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress, _, _));
    nfcInstance->readTag(wrongBlockAddress, buffer);
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tryOverwriteTrailerBlock_autoCorrectedOnLogin)
{
    uint8_t wrongBlockAddress = 7;
    uint8_t automaticallyCorrectedTrailerBlock = 11;
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    EXPECT_CALL(m_Mfrc, tagLogin(automaticallyCorrectedTrailerBlock)).WillOnce(Return(false));
    nfcInstance->readTag(wrongBlockAddress, buffer);
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tryOverwriteTrailerBlock_automaticallyCorrectedOnRead)
{
    uint8_t wrongBlockAddress = 7;
    uint8_t automaticallyCorrectedBlockAddress = 8;
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress, _, _));
    nfcInstance->readTag(wrongBlockAddress, buffer);
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tagReadCalledReturnsTrue)
{
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_TRUE(nfcInstance->readTag(1, buffer));
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tagReadCalled_ReturnsCorrectBufferData)
{
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    nfcInstance->readTag(1, buffer);
    EXPECT_TRUE(resultArrayByteCompare(fakeBufferData, buffer, NFCTAG_MEMORY_TO_OCCUPY));
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tagReadCalled_callsTagHaltOnCompletion)
{
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagHalt());
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_readTag, MifareUltralight_setOnlineFails_returnsFalse)
{
    m_Mfrc.DelegateToFakeUltralight();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(false));
    EXPECT_CALL(m_Mfrc, tagRead(_, _, _)).Times(0);
    ASSERT_EQ(false, nfcInstance->readTag(4, buffer));
}

TEST_F(NfcTag_readTag, MifareUltralight_tagReadCalledWithCorrectArguments)
{
    m_Mfrc.DelegateToFakeUltralight();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    // read 4 consecutive 4 byte blocks
    EXPECT_CALL(m_Mfrc, tagRead(4, _, NFCTAG_MEMORY_TO_OCCUPY + 2));
    EXPECT_CALL(m_Mfrc, tagRead(5, _, NFCTAG_MEMORY_TO_OCCUPY + 2));
    EXPECT_CALL(m_Mfrc, tagRead(6, _, NFCTAG_MEMORY_TO_OCCUPY + 2));
    EXPECT_CALL(m_Mfrc, tagRead(7, _, NFCTAG_MEMORY_TO_OCCUPY + 2));
    nfcInstance->readTag(4, buffer);
}

TEST_F(NfcTag_readTag, MifareUltralight_wrongBlockAddress_automaticallyCorrected)
{
    uint8_t wrongBlockAddress = 3;
    uint8_t automaticallyCorrectedBlockAddress = 4;
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress, _, _));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress + 1, _, _));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress + 2, _, _));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress + 3, _, _));
    nfcInstance->readTag(wrongBlockAddress, buffer);
}

TEST_F(NfcTag_readTag, MifareUltralight_outOfRangeBlockAddress_automaticallyCorrected)
{
    uint8_t wrongBlockAddress = 12;
    uint8_t automaticallyCorrectedBlockAddress = 11;
    m_Mfrc.DelegateToFakeMini1k4k();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress, _, _));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress + 1, _, _));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress + 2, _, _));
    EXPECT_CALL(m_Mfrc, tagRead(automaticallyCorrectedBlockAddress + 3, _, _));
    nfcInstance->readTag(wrongBlockAddress, buffer);
}

TEST_F(NfcTag_readTag, MifareUltralight_tagReadCalledReturnsTrue)
{
    m_Mfrc.DelegateToFakeUltralight();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_TRUE(nfcInstance->readTag(4, buffer));
}

TEST_F(NfcTag_readTag, MifareUltralight_tagReadCalled_ReturnsCorrectBufferData)
{
    m_Mfrc.DelegateToFakeUltralight();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    nfcInstance->readTag(4, buffer);
    EXPECT_TRUE(resultArrayByteCompare(fakeBufferData, buffer, NFCTAG_MEMORY_TO_OCCUPY));
}

TEST_F(NfcTag_readTag, MifareUltralight_tagReadCalled_callsTagHaltOnCompletion)
{
    m_Mfrc.DelegateToFakeUltralight();
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagHalt());
    nfcInstance->readTag(4, buffer);
}

TEST_F(NfcTag_writeTag, MifareMini1k4k_tagLoginFails_writeReturnsFalse)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(false));
    EXPECT_CALL(m_Mfrc, tagWrite).Times(0);
    nfcInstance->writeTag(4, buffer);
}

TEST_F(NfcTag_writeTag, MifareMini1k4k_tagWriteCalled_withCorrectArguments)
{

    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagWrite(1, _, NFCTAG_MEMORY_TO_OCCUPY));
    nfcInstance->writeTag(1, writeBuffer);
}

TEST_F(NfcTag_writeTag, MifareMini1k4k_tagWriteCalled_withCorrectPayload)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagWrite(_, arrayByteCompare(fakeBufferData, NFCTAG_MEMORY_TO_OCCUPY), _));
    nfcInstance->writeTag(1, writeBuffer);
}

TEST_F(NfcTag_writeTag, MifareMini1k4k_tagWriteCalled_callsTagHaltOnCompletion)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc, tagWrite(_, _, _)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagHalt());
    nfcInstance->writeTag(1, writeBuffer);
}

TEST_F(NfcTag_writeTag, MifareMini1k4k_tagWriteCalled_returnsTrue)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc, tagWrite(_, _, _)).WillByDefault(Return(true));
    EXPECT_EQ(true, nfcInstance->writeTag(1, writeBuffer));
}

TEST_F(NfcTag_writeTag, MifareUltralight_tagLoginFails_writeReturnsFalse)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    byte tgtArray[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte *buffer{tgtArray};
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(false));
    EXPECT_CALL(m_Mfrc, tagWrite).Times(0);
    nfcInstance->writeTag(4, buffer);
}

TEST_F(NfcTag_writeTag, MifareUltralight_tagWriteCalled_withCorrectArguments)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    // writes 4 consecutive 4 byte blocks
    EXPECT_CALL(m_Mfrc, tagWrite(4, _, NFCTAG_MEMORY_TO_OCCUPY));
    EXPECT_CALL(m_Mfrc, tagWrite(5, _, NFCTAG_MEMORY_TO_OCCUPY));
    EXPECT_CALL(m_Mfrc, tagWrite(6, _, NFCTAG_MEMORY_TO_OCCUPY));
    EXPECT_CALL(m_Mfrc, tagWrite(7, _, NFCTAG_MEMORY_TO_OCCUPY));
    nfcInstance->writeTag(4, writeBuffer);
}

TEST_F(NfcTag_writeTag, MifareUltralight_tagWriteCalled_withCorrectPayload)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    uint8_t tB0[4] = {};
    uint8_t tB1[4] = {};
    uint8_t tB2[4] = {};

    tB0[0] = writeBuffer[0]; // Byte x of block
    tB0[1] = writeBuffer[1]; // Byte x of block
    tB0[2] = writeBuffer[2]; // Byte x of block
    tB0[3] = writeBuffer[3]; // Byte x of block
    tB1[0] = writeBuffer[4]; // Byte x of block
    tB1[1] = writeBuffer[5]; // Byte x of block
    tB1[2] = writeBuffer[6]; // Byte x of block
    tB1[3] = writeBuffer[7]; // Byte x of block
    EXPECT_CALL(m_Mfrc, tagWrite(4, arrayByteCompare(tB0, 4), _));
    EXPECT_CALL(m_Mfrc, tagWrite(5, arrayByteCompare(tB1, 4), _));
    EXPECT_CALL(m_Mfrc, tagWrite(6, arrayByteCompare(tB2, 4), _));
    EXPECT_CALL(m_Mfrc, tagWrite(7, arrayByteCompare(tB2, 4), _));
    nfcInstance->writeTag(4, writeBuffer);
}

TEST_F(NfcTag_writeTag, MifareUltralight_tagWriteCalled_callsTagHaltOnCompletion)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc, tagWrite(_, _, _)).WillByDefault(Return(true));
    EXPECT_CALL(m_Mfrc, tagHalt());
    nfcInstance->writeTag(4, writeBuffer);
}

TEST_F(NfcTag_writeTag, MifareUltralight_tagWriteCalled_returnsTrue)
{
    ON_CALL(m_Mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_Mfrc);
    ON_CALL(m_Mfrc, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc, tagWrite(_, _, _)).WillByDefault(Return(true));
    EXPECT_EQ(true, nfcInstance->writeTag(4, writeBuffer));
}