#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Nfc_mocks.h"
#include "mocks/unittest_ErrorHandler_mocks.h"

#include "../Nfc/Nfc_implementation/Nfc_implementation.h"

// This file tests the Nfc implementation
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class Nfc_getTagPresence : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pNfc = new Nfc_implementation(&m_mfrc, &m_messageHandler);
    }

    virtual void TearDown()
    {
        delete m_pNfc;
    }

protected:
    NiceMock<Mock_MFRC522> m_mfrc{};
    NiceMock<Mock_MessageHandler> m_messageHandler{};
    Nfc_implementation *m_pNfc{nullptr};
};

class Nfc_write : public Nfc_getTagPresence{};
class Nfc_read : public Nfc_getTagPresence{};

// TESTS
TEST_F(Nfc_getTagPresence, init_callsReadersInit)
{
    EXPECT_CALL(m_mfrc, init());
    m_pNfc->initNfc();
}

TEST_F(Nfc_getTagPresence, noTag_returnsNO_TAG)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(false));
    ASSERT_EQ(Nfc_interface::NO_TAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, knownTag_returnsACTIVE_KNOWN_TAG)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, isNewCardPresent()).WillByDefault(Return(false));
    ASSERT_EQ(Nfc_interface::ACTIVE_KNOWN_TAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, newTag_returnsNEW_UNKNOWN_TAG)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, isNewCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ASSERT_EQ(Nfc_interface::NEW_UNKNOWN_TAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, canNotSetTagOnline_returnsERROR)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, isNewCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_NOT_COMPLETE));

    EXPECT_CALL(m_messageHandler, printMessage(NfcNotify::toString(NfcNotify::tagTypeNotImplementedError)));
    m_pNfc->getTagPresence();
}

TEST_F(Nfc_write, getTagFails_writeNotCalled)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_UNKNOWN));
    uint8_t dataToWrite[16] = {};
    EXPECT_CALL(m_mfrc, tagWrite(_, _, _)).Times(0);
    ASSERT_EQ(false, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, mfrcWriteFails_returnsFalse)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_mfrc, tagWrite(_, _, _)).WillOnce(Return(false));
    uint8_t dataToWrite[16] = {};
    ASSERT_EQ(false, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, mfrcWriteSucceeds_returnsTrue)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_mfrc, tagWrite(_, _, _)).WillByDefault(Return(true));
    uint8_t dataToWrite[16] = {};
    ASSERT_EQ(true, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, tagWriteError_returnsError)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagWrite(_, _, _)).WillByDefault(Return(false));
    uint8_t dataToWrite[16] = {};

    EXPECT_CALL(m_messageHandler, printMessage(NfcNotify::toString(NfcNotify::tagWriteError)));
    m_pNfc->writeTag(4, dataToWrite);
}

TEST_F(Nfc_write, tagWriteSuccess_returnsSuccessNotification)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagWrite(_, _, _)).WillByDefault(Return(true));
    uint8_t dataToWrite[16] = {};

    EXPECT_CALL(m_messageHandler, printMessage(NfcNotify::toString(NfcNotify::tagWriteSuccess)));
    m_pNfc->writeTag(4, dataToWrite);
}

TEST_F(Nfc_read, getTagFails_readNotCalled)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_UNKNOWN));
    uint8_t dataToRead[16] = {};
    EXPECT_CALL(m_mfrc, tagRead(_, _, _)).Times(0);
    ASSERT_EQ(false, m_pNfc->readTag(4, dataToRead));
}

TEST_F(Nfc_read, mfrcReadFails_returnsFalse)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_mfrc, tagRead(_, _, _)).WillOnce(Return(false));
    uint8_t dataToRead[16] = {};
    ASSERT_EQ(false, m_pNfc->readTag(4, dataToRead));
}

TEST_F(Nfc_read, mfrcReadSucceeds_returnsTrue)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_mfrc, tagRead(_, _, _)).WillByDefault(Return(true));
    uint8_t dataToRead[16] = {};
    ASSERT_EQ(true, m_pNfc->readTag(4, dataToRead));
}

TEST_F(Nfc_read, tagReadError_returnsError)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagRead(_, _, _)).WillByDefault(Return(false));
    uint8_t readData[16] = {};
    
    EXPECT_CALL(m_messageHandler, printMessage(NfcNotify::toString(NfcNotify::tagReadError)));
    m_pNfc->readTag(4, readData);
}

TEST_F(Nfc_read, tagReadSuccess_returnsSuccessNotification)
{
    ON_CALL(m_mfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrc, tagRead(_, _, _)).WillByDefault(Return(true));
    uint8_t readData[16] = {};

    EXPECT_CALL(m_messageHandler, printMessage(NfcNotify::toString(NfcNotify::tagReadSuccess)));
    m_pNfc->readTag(4, readData);
}

