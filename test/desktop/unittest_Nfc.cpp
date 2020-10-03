#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Nfc_mocks.h"
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
        m_pMfrc = new NiceMock<Mock_MFRC522>;
        m_pNfc = new Nfc_implementation(m_pMfrc);
    }

    virtual void TearDown()
    {
        delete m_pNfc;
        delete m_pMfrc;
    }

protected:
    NiceMock<Mock_MFRC522> *m_pMfrc;
    Nfc_implementation *m_pNfc{nullptr};
};

class Nfc_write : public Nfc_getTagPresence{};

// TESTS
TEST_F(Nfc_getTagPresence, noTag_returnsNO_TAG)
{
    ON_CALL(*m_pMfrc, isCardPresent()).WillByDefault(Return(false));
    EXPECT_EQ(Nfc_interface::NO_TAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, knownTag_returnsACTIVE_KNOWN_TAG)
{
    ON_CALL(*m_pMfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, isNewCardPresent()).WillByDefault(Return(false));
    EXPECT_EQ(Nfc_interface::ACTIVE_KNOWN_TAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, newTag_returnsNEW_UNKNOWN_TAG)
{
    ON_CALL(*m_pMfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, isNewCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    EXPECT_EQ(Nfc_interface::NEW_UNKNOWN_TAG, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, cannotSetTagOnline_returnsERROR)
{
    ON_CALL(*m_pMfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, isNewCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_NOT_COMPLETE));
    EXPECT_EQ(Nfc_interface::ERROR, m_pNfc->getTagPresence());
}

TEST_F(Nfc_getTagPresence, tagWriteFailed_returnsERROR) // hier weitermachen!
{
    ON_CALL(*m_pMfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, isNewCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, tagWrite(_, _)).WillByDefault(Return(false));
    EXPECT_EQ(Nfc_interface::ERROR, m_pNfc->getTagPresence());
}

TEST_F(Nfc_write, getFactoryInstance_Fails_writeFails) // hier weitermachen!
{
    ON_CALL(*m_pMfrc, isCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, isNewCardPresent()).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, tagWrite(_, _)).WillByDefault(Return(false));
    EXPECT_EQ(Nfc_interface::ERROR, m_pNfc->getTagPresence());
}
