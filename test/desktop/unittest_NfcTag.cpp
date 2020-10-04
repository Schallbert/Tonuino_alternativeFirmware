#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Nfc_mocks.h"
#include "../Nfc/NfcTag_implementation/NfcTag_factory.h"

// This file tests the Nfc implementation
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class NfcTag_getInstance : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pMfrc = new NiceMock<Mock_MFRC522>;
    }

    virtual void TearDown()
    {
        delete m_pMfrc;
    }

protected:
    NiceMock<Mock_MFRC522> *m_pMfrc;
};

class NfcTag_readTag : public NfcTag_getInstance{};
class NfcTag_writeTag : public NfcTag_getInstance{};

TEST_F(NfcTag_getInstance, TagTypeUNKNOWN_returnsNullptr)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_UNKNOWN));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_pMfrc));
}

TEST_F(NfcTag_getInstance, TagTypeISO14443_returnsNullptr)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_ISO_14443_4));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_pMfrc));
}

TEST_F(NfcTag_getInstance, TagTypeISO18092_returnsNullptr)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_ISO_18092));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_pMfrc));
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREMINI_returnsInstance)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_MINI));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_pMfrc);
    EXPECT_CALL(*m_pMfrc, tagLogin(_)).WillOnce(Return(false));
    byte* buffer{nullptr};
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFARE1k_returnsInstance)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_MINI));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_pMfrc);
    EXPECT_CALL(*m_pMfrc, tagLogin(_)).WillOnce(Return(false));
    byte* buffer{nullptr};
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFARE4k_returnsInstance)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_4K));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_pMfrc);
    EXPECT_CALL(*m_pMfrc, tagLogin(_)).WillOnce(Return(false));
    byte* buffer{nullptr};
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREUL_returnsInstance)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_UL));
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_pMfrc);
    EXPECT_CALL(*m_pMfrc, tagLogin(_)).WillOnce(Return(false));
    byte* buffer{nullptr};
    nfcInstance->readTag(4, buffer);
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREUL_returnsNullptr)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_PLUS));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_pMfrc));
}

TEST_F(NfcTag_getInstance, TagTypeMIFAREDESFIRE_returnsNullptr)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_DESFIRE));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_pMfrc));
}

TEST_F(NfcTag_getInstance, TagTypeTNP3XXX_returnsNullptr)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_TNP3XXX));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_pMfrc));
}

TEST_F(NfcTag_getInstance, TagTypeINCOMPLETE_returnsNullptr)
{
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_NOT_COMPLETE));
    EXPECT_EQ(nullptr, NfcTag_factory::getInstance(m_pMfrc));
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tagReadCalledWithCorrectArguments)
{
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_pMfrc);
     byte tgtArray[16] = {};
    byte* buffer{tgtArray};
    ON_CALL(*m_pMfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(*m_pMfrc, tagRead(1, _, 18));
    nfcInstance->readTag(1, buffer);
}

TEST_F(NfcTag_readTag, MifareMini1k4k_tagReadCalledWithCorrectArguments)
{
    NfcTag_interface *nfcInstance = NfcTag_factory::getInstance(m_pMfrc);
     byte tgtArray[16] = {};
    byte* buffer{tgtArray};
    ON_CALL(*m_pMfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(*m_pMfrc, tagRead(1, _, 18));
    nfcInstance->readTag(1, buffer);
}