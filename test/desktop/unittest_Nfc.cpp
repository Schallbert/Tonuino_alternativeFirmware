#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Nfc_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "../Nfc/Nfc/Nfc_implementation.h"

// This file tests the Nfc implementation
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Sequence;

// TEST FIXTURE
class Nfc_getTagPresence : public ::testing::Test
{
    virtual void SetUp()
    {
        m_pNfc = new Nfc_implementation(m_mfrcMock, m_messageHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pNfc;
        m_pNfc = nullptr;
    }

protected:
    NiceMock<Mock_MFRC522> m_mfrcMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    Nfc_implementation *m_pNfc{nullptr};
};

class Nfc_write : public Nfc_getTagPresence
{
};
class Nfc_read : public Nfc_getTagPresence
{
};

// TESTS
TEST_F(Nfc_getTagPresence, init_callsReadersInit)
{
    EXPECT_CALL(m_mfrcMock, init());
    m_pNfc->initNfc();
}

TEST_F(Nfc_getTagPresence, noTag_returnsNO_TAG)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(false));
    ASSERT_EQ(Message::NOTAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, knownTag_returnsACTIVE_KNOWN_TAG)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    m_pNfc->getTagPresence();                                // Will return NEWTAG
    ASSERT_EQ(Message::ACTIVETAG, m_pNfc->getTagPresence()); // when still present, tag is reported ACTIVE
}

TEST_F(Nfc_getTagPresence, newTag_returnsUNKNOWN_TAG)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ASSERT_EQ(Message::UNKNOWNTAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_write, noTagPresent_writeNotCalled)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(false));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    uint8_t dataToWrite[16] = {};
    EXPECT_CALL(m_mfrcMock, tagWrite(_, _, _)).Times(0);
    ASSERT_EQ(false, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, tagSetActiveFails_writeNotCalled)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(false));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    uint8_t dataToWrite[16] = {};
    EXPECT_CALL(m_mfrcMock, tagWrite(_, _, _)).Times(0);
    ASSERT_EQ(false, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, getTagUidFails_writeNotCalled)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(false));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    uint8_t dataToWrite[16] = {};
    EXPECT_CALL(m_mfrcMock, tagWrite(_, _, _)).Times(0);
    ASSERT_EQ(false, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, getTagTypeFails_writeNotCalled)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_UNKNOWN));
    uint8_t dataToWrite[16] = {};
    EXPECT_CALL(m_mfrcMock, tagWrite(_, _, _)).Times(0);
    ASSERT_EQ(false, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, mfrcWriteFails_returnsFalse)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_mfrcMock, tagWrite(_, _, _)).WillOnce(Return(false));
    uint8_t dataToWrite[16] = {};
    ASSERT_EQ(false, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, mfrcWriteSucceeds_returnsTrue)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, tagWrite(_, _, _)).WillByDefault(Return(true));
    uint8_t dataToWrite[16] = {};
    ASSERT_EQ(true, m_pNfc->writeTag(4, dataToWrite));
}

TEST_F(Nfc_write, tagWriteError_returnsError)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagWrite(_, _, _)).WillByDefault(Return(false));
    uint8_t dataToWrite[16] = {};

    Sequence seq;
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGONLINE)));
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGERRORWRITE)));
    m_pNfc->writeTag(4, dataToWrite);
}

TEST_F(Nfc_write, tagWriteSuccess_returnsSuccessNotification)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, tagWrite(_, _, _)).WillByDefault(Return(true));
    uint8_t dataToWrite[16] = {};

    Sequence seq;
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGONLINE)));
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGWRITEOK)));
    m_pNfc->writeTag(4, dataToWrite);
}

TEST_F(Nfc_read, getTagFails_readNotCalled)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_UNKNOWN));
    uint8_t dataToRead[16] = {};
    EXPECT_CALL(m_mfrcMock, tagRead(_, _, _)).Times(0);
    ASSERT_EQ(false, m_pNfc->readTag(4, dataToRead));
}

TEST_F(Nfc_read, mfrcReadFails_returnsFalse)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagLogin(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_mfrcMock, tagRead(_, _, _)).WillOnce(Return(false));
    uint8_t dataToRead[16] = {};
    ASSERT_EQ(false, m_pNfc->readTag(4, dataToRead));
}

TEST_F(Nfc_read, mfrcReadSucceeds_returnsTrue)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, tagRead(_, _, _)).WillByDefault(Return(true));
    uint8_t dataToRead[16] = {};
    ASSERT_EQ(true, m_pNfc->readTag(4, dataToRead));
}

TEST_F(Nfc_read, tagReadError_returnsError)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagRead(_, _, _)).WillByDefault(Return(false));
    uint8_t readData[16] = {};

    // setOnline will return TAGREADOK, tagRead will fail with TAGERRORREAD
    Sequence seq;
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGONLINE)));
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGERRORREAD)));
    m_pNfc->readTag(4, readData);
}

TEST_F(Nfc_read, tagReadSuccess_returnsSuccessNotification)
{
    ON_CALL(m_mfrcMock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagUid()).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_mfrcMock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_mfrcMock, tagRead(_, _, _)).WillByDefault(Return(true));
    uint8_t readData[16] = {};

    Sequence seq;
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGONLINE)));
    EXPECT_CALL(m_messageHandlerMock, printMessage(identicalMessage(Message::TAGREADOK)));
    m_pNfc->readTag(4, readData);
}
