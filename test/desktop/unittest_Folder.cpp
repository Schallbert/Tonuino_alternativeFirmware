#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Folder.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST_F FIXTURE
// This fixture will provide the dependency container
// (which actually is empty!)
class folderInvalid : public ::testing::Test
{
protected:
    NiceMock<Mock_ArduinoDIcontainer> m_arduinoHalMock;
    NiceMock<Mock_MessageHandler> m_messageHandlerMock;
};

// This fixture will return Mock_eeprom dependency
class folderMethods : public folderInvalid
{
protected:
    virtual void SetUp()
    {
        folderInvalid::SetUp();
        m_pEeprom = new NiceMock<Mock_eeprom>;
        folderInvalid::m_arduinoHalMock.DelegateToMockEeprom(m_pEeprom);
    }

    virtual void TearDown()
    {
        delete m_pEeprom;
        folderInvalid::TearDown();
    }

protected:
    NiceMock<Mock_eeprom> *m_pEeprom{nullptr};
};

// This fixture will return Mock_eeprom dependency
// and a valid folder object
class folderAlbum : public folderMethods
{
protected:
    virtual void SetUp()
    {
        folderMethods::SetUp();
        pValidFolder = new Folder(2, Folder::ALBUM);
        pValidFolder->setTrackCount(10);
        pValidFolder->setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    }

    virtual void TearDown()
    {
        delete pValidFolder;
        folderMethods::TearDown();
    }

protected:
    Folder *pValidFolder{nullptr};
};

// This fixture will both return a Mock_eeprom and Mock_Random
// from Dependency container
class folderDependencies : public folderMethods
{
protected:
    virtual void SetUp()
    {
        folderMethods::SetUp();
        m_pRandom = new NiceMock<Mock_random>;
        folderMethods::m_arduinoHalMock.DelegateToMockRandom(m_pRandom);
    }

    virtual void TearDown()
    {
        delete m_pRandom;
        folderMethods::TearDown();
    }

protected:
    NiceMock<Mock_random> *m_pRandom{nullptr};
};

// TESTS
TEST_F(folderInvalid, folder_defaultConstructor_isInvalid)
{
    Folder invalidFolder;
    ASSERT_EQ(false, invalidFolder.isValid());
}

TEST_F(folderInvalid, folderIdIs0)
{
    Folder invalidFolder;
    ASSERT_EQ(0, invalidFolder.getFolderId());
}

TEST_F(folderInvalid, playModeIsUndefined)
{
    Folder invalidFolder;
    ASSERT_EQ(Folder::UNDEFINED, invalidFolder.getPlayMode());
}

TEST_F(folderInvalid, trackCountIs0)
{
    Folder invalidFolder;
    ASSERT_EQ(0, invalidFolder.getTrackCount());
}

TEST_F(folderInvalid, currentTrackIs0)
{
    Folder invalidFolder;
    ASSERT_EQ(0, invalidFolder.getCurrentTrack());
}

TEST_F(folderInvalid, nextTrackIs0)
{
    Folder invalidFolder;
    ASSERT_EQ(0, invalidFolder.getNextTrack());
}

TEST_F(folderInvalid, prevTrackIs0)
{
    Folder invalidFolder;
    ASSERT_EQ(0, invalidFolder.getPrevTrack());
}

TEST_F(folderInvalid, initiated_isInitialized_returnsTrue)
{
    Folder invalidFolder(1, Folder::ALBUM);
    ASSERT_TRUE(invalidFolder.isInitiated());
}

TEST_F(folderInvalid, initiated_isValid_returnsFalse)
{
    Folder invalidFolder(1, Folder::ALBUM);
    ASSERT_FALSE(invalidFolder.isValid());
}

TEST_F(folderInvalid, initiated_trackCountSet_isValid_returnsFalse)
{
    Folder invalidFolder(1, Folder::SAVEPROGRESS);
    invalidFolder.setTrackCount(2);
    ASSERT_FALSE(invalidFolder.isValid());
}

TEST_F(folderInvalid, dependenciesSetup_isValidReturnsFalse)
{
    Folder invalidFolder(1, Folder::ALBUM);
    invalidFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    ASSERT_FALSE(invalidFolder.isValid());
}

TEST_F(folderInvalid, playModeUndefined_isValidReturnsFalse)
{
    Folder invalidFolder(1, Folder::UNDEFINED);
    invalidFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    ASSERT_FALSE(invalidFolder.isValid());
}

TEST_F(folderInvalid, folderIdIs0_isValidReturnsFalse)
{
    Folder invalidFolder(0, Folder::ALBUM);
    invalidFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    ASSERT_FALSE(invalidFolder.isValid());
}

TEST_F(folderMethods, copyConstructor_workingOK)
{
    // Arrange
    ON_CALL(*m_pEeprom, eeprom_read(254)).WillByDefault(Return(13));
    Folder testFolder(254, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(1);
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    // Act
    Folder copyFolder(testFolder);
    // Assert
    ASSERT_TRUE(copyFolder.isValid());
}

TEST_F(folderMethods, copyConstructor_dependencyIsNullptr_isAlsoCopied)
{
    // Arrange
    Folder testFolder(254, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(1);
    // Act
    Folder copyFolder(testFolder);
    // Assert
    ASSERT_FALSE(testFolder.isValid());
    ASSERT_FALSE(copyFolder.isValid());
}

TEST_F(folderMethods, copyConstructor_valuesOK)
{
    ON_CALL(*m_pEeprom, eeprom_read(254)).WillByDefault(Return(13));
    Folder testFolder(254, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(14);
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);

    Folder copyFolder(testFolder);

    // Validate that contents is copied OK
    EXPECT_EQ(13, copyFolder.getCurrentTrack());
    EXPECT_EQ(254, copyFolder.getFolderId());
    EXPECT_EQ(Folder::SAVEPROGRESS, copyFolder.getPlayMode());
    EXPECT_EQ(14, copyFolder.getTrackCount());
}

TEST_F(folderMethods, assignmentOperator_workingOK)
{
    Folder testFolder(254, Folder::ALBUM);
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    testFolder.setTrackCount(1);
    Folder copyFolder = Folder();

    copyFolder = testFolder;
    // Check if variables are set OK
    EXPECT_TRUE(copyFolder.isValid());
}

TEST_F(folderMethods, assignmentOperator_valuesOK)
{
    Folder testFolder(254, Folder::ALBUM);
    testFolder.setTrackCount(1);
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    Folder copyFolder = Folder();

    copyFolder = testFolder;

    // Validate that contents is copied OK
    EXPECT_EQ(1, copyFolder.getCurrentTrack());
    EXPECT_EQ(254, copyFolder.getFolderId());
    EXPECT_EQ(Folder::ALBUM, copyFolder.getPlayMode());
    EXPECT_EQ(1, copyFolder.getTrackCount());
}

TEST_F(folderMethods, assignmentOperator_dependencyIsNullptr_isAlsoCopied)
{
    // Arrange
    Folder testFolder(254, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(1);
    // Act
    Folder copyFolder = Folder();
    copyFolder = testFolder;
    // Assert
    ASSERT_FALSE(copyFolder.isValid());
}

TEST_F(folderMethods, validFolder_createsQueue_callsNotification)
{
    Folder testFolder(254, Folder::ALBUM);
    testFolder.setTrackCount(1);
    
    EXPECT_CALL(m_messageHandlerMock, printMessage(_));
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
}

TEST_F(folderAlbum, folder_ALBUM_valid)
{
    ASSERT_TRUE(pValidFolder->isValid());
}

TEST_F(folderAlbum, ALBUM_idIs2)
{
    ASSERT_EQ(2, pValidFolder->getFolderId());
}

TEST_F(folderAlbum, playModeIsALBUM)
{
    ASSERT_EQ(Folder::ALBUM, pValidFolder->getPlayMode());
}

TEST_F(folderAlbum, trackCountIs10)
{
    ASSERT_EQ(10, pValidFolder->getTrackCount());
}

TEST_F(folderAlbum, currentTrackIs1)
{
    ASSERT_EQ(1, pValidFolder->getCurrentTrack());
}

TEST_F(folderAlbum, nextTrackIs2)
{
    ASSERT_EQ(2, pValidFolder->getNextTrack());
}

TEST_F(folderAlbum, ChooseNextTrack_currentTrackIs2)
{
    pValidFolder->getNextTrack();
    ASSERT_EQ(2, pValidFolder->getCurrentTrack());
}

TEST_F(folderAlbum, trackCount10_CallPrevTrack_returns10)
{
    // Tests rollover track count
    ASSERT_EQ(10, pValidFolder->getPrevTrack());
}

TEST_F(folderAlbum, trackCount10_CallPrevTrack_GetCurrentTrackReturns10)
{
    // Tests rollover track count
    pValidFolder->getPrevTrack();
    ASSERT_EQ(10, pValidFolder->getCurrentTrack());
}

TEST_F(folderAlbum, trackCountIs10_AllTracksAreInQueue)
{
    uint8_t trackSum = pValidFolder->getCurrentTrack();
    for (int i = 2; i <= pValidFolder->getTrackCount(); ++i)
    {
        trackSum += pValidFolder->getNextTrack();
    }
    ASSERT_EQ(55, trackSum);
}

TEST_F(folderDependencies, RANDOM_trackCountIs10_trackQueueLoopComplete)
{
    Folder testFolder(1, Folder::RANDOM);
    testFolder.setTrackCount(10);
    // fake response, so shouldn't lock up in while loop
    EXPECT_CALL(*m_pRandom, random_generateUi8)
        .WillOnce(Return(1))
        .WillOnce(Return(10))
        .WillOnce(Return(2))
        .WillOnce(Return(9))
        .WillOnce(Return(3))
        .WillOnce(Return(8))
        .WillOnce(Return(4))
        .WillOnce(Return(7))
        .WillOnce(Return(5))
        .WillOnce(Return(6));
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    // take sample that track queue is correctly setup
    // ALSO CONFIRMS THAT TRACK LIST IS OK BECAUSE OF WILLONCE STRUCTURE
    EXPECT_EQ(1, testFolder.getCurrentTrack());
    EXPECT_EQ(10, testFolder.getNextTrack());
}

TEST_F(folderDependencies, RANDOM_trackCountIs10_willNotAllowSameTrackMultipleTimesInQueue)
{
    Folder testFolder(1, Folder::RANDOM);
    testFolder.setTrackCount(10);
    // fake response, so shouldn't lock up in while loop
    EXPECT_CALL(*m_pRandom, random_generateUi8)
        .WillOnce(Return(1))
        .WillOnce(Return(10))
        .WillOnce(Return(10)) // Duplicate
        .WillOnce(Return(2))
        .WillOnce(Return(9))
        .WillOnce(Return(3))
        .WillOnce(Return(10)) // Duplicate
        .WillOnce(Return(8))
        .WillOnce(Return(4))
        .WillOnce(Return(7))
        .WillOnce(Return(5))
        .WillOnce(Return(6));

    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    // take sample that track queue is correctly setup
    testFolder.getNextTrack();
    ASSERT_EQ(2, testFolder.getNextTrack());
}

TEST_F(folderDependencies, RANDOM_trackCountIs10_willNotAllowTrackNumber0)
{
    Folder testFolder(1, Folder::RANDOM);
    testFolder.setTrackCount(10);
    // fake response, so shouldn't lock up in while loop
    EXPECT_CALL(*m_pRandom, random_generateUi8)
        .WillOnce(Return(1))
        .WillOnce(Return(0)) // Not allowed
        .WillOnce(Return(10))
        .WillOnce(Return(2))
        .WillOnce(Return(9))
        .WillOnce(Return(3))
        .WillOnce(Return(0)) // Not allowed
        .WillOnce(Return(8))
        .WillOnce(Return(4))
        .WillOnce(Return(7))
        .WillOnce(Return(5))
        .WillOnce(Return(6));

    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    // take sample that track queue is correctly setup
    ASSERT_EQ(10, testFolder.getNextTrack());
}

TEST_F(folderDependencies, RANDOM_trackCountIs10_willNotAllowTrackNumberOutOfRange)
{
    Folder testFolder(1, Folder::RANDOM);
    testFolder.setTrackCount(10);
    // fake response, so shouldn't lock up in while loop
    EXPECT_CALL(*m_pRandom, random_generateUi8)
        .WillOnce(Return(1))
        .WillOnce(Return(28)) // Not allowed
        .WillOnce(Return(10))
        .WillOnce(Return(2))
        .WillOnce(Return(9))
        .WillOnce(Return(3))
        .WillOnce(Return(28)) // Not allowed
        .WillOnce(Return(8))
        .WillOnce(Return(4))
        .WillOnce(Return(7))
        .WillOnce(Return(5))
        .WillOnce(Return(6));

    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    // take sample that track queue is correctly setup
    ASSERT_EQ(10, testFolder.getNextTrack());
}

TEST_F(folderDependencies, SAVEPROGRESS_trackLoadIsWorking)
{
    Folder testFolder(99, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(16);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WillByDefault(Return(13));
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);

    EXPECT_EQ(13, testFolder.getCurrentTrack());
}

TEST_F(folderDependencies, SAVEPROGRESS_trackLoadValueIsCorrupt_ReturnsTrack1)
{
    Folder testFolder(99, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(10);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WillByDefault(Return(13));
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);

    EXPECT_EQ(1, testFolder.getCurrentTrack());
}

TEST_F(folderDependencies, SAVEPROGRESS_trackSaveIsWorking)
{
    Folder testFolder(99, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(16);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WillByDefault(Return(13));

    EXPECT_CALL(*m_pEeprom, eeprom_write(99, 14));
    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    EXPECT_EQ(14, testFolder.getNextTrack());
}

TEST_F(folderDependencies, SAVEPROGRESS_EepromRead0_currentTrackDefaultsTo1)
{
    Folder testFolder(99, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(16);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WillByDefault(Return(0));

    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    EXPECT_EQ(1, testFolder.getCurrentTrack());
}
TEST_F(folderDependencies, SAVEPROGRESS_EepromReadOutOfRange_currentTrackDefaultsTo1)
{
    Folder testFolder(99, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(16);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WillByDefault(Return(17));

    testFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    EXPECT_EQ(1, testFolder.getCurrentTrack());
}
