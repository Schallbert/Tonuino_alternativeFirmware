#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Folder.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"

using ::testing::NiceMock;
using ::testing::Return;

// TEST_F FIXTURE
// This fixture will provide the dependency container
// (which actually is empty!)
class folderInvalid : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pADIC = new NiceMock<Mock_ArduinoDIcontainer>;
    }

    virtual void TearDown()
    {
        delete m_pADIC;
    }

protected:
    NiceMock<Mock_ArduinoDIcontainer> *m_pADIC{nullptr};
};

// This fixture will return Mock_eeprom dependency
class folderMethods : public folderInvalid
{
    virtual void SetUp()
    {
        folderInvalid::SetUp();
        m_pEeprom = new NiceMock<Mock_eeprom>;
        folderInvalid::m_pADIC->DelegateToMockEeprom(m_pEeprom);
    }

    virtual void TearDown()
    {
        delete m_pEeprom;
        folderInvalid.TearDown();
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
        testFolder = new Folder((2, Folder::ALBUM, 10);
        testFolder->setup_dependencies(m_pADIC);
    }

    virtual void TearDown()
    {
        delete testFolder;
        folderMethods::TearDown();
    }

protected:
    Folder *testFolder{nullptr};
};

// This fixture will both return a Mock_eeprom and Mock_Random
// from Dependency container
class folderDependencies : public folderMethods
{
protected:
    virtual void SetUp()
    {
        folderInvalid::SetUp();
        m_pRandom = new NiceMock<Mock_random>;
        folderInvalid::m_pADIC->DelegateToMockRandom(m_pRandom);
    }

    virtual void TearDown()
    {
        delete m_pRandom;
        folderInvalid.TearDown();
    }

protected:
    NiceMock<Mock_random> *m_pRandom{nullptr};
};

// TESTS
TEST_F(folderInvalid, folder_defaultConstructor_isInvalid)
{
    Folder testFolder;
    ASSERT_EQ(false, testFolder.is_valid());
}
TEST_F(folderInvalid, folderIdIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder.get_folder_id());
}
TEST_F(folderInvalid, playModeIsUndefined)
{
    Folder testFolder;
    ASSERT_EQ(Folder::UNDEFINED, testFolder->get_play_mode());
}
TEST_F(folderInvalid, trackCountIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder->get_track_count());
}
TEST_F(folderInvalid, currentTrackIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder->get_current_track());
}
TEST_F(folderInvalid, nextTrackIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder->get_next_track());
}
TEST_F(folderInvalid, prevTrackIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder->get_prev_track());
}
TEST_F(folderInvalid, dependenciesSetup_isValidReturnsFalse)
{
    Folder testFolder(1, Folder::ALBUM, 0);
    testFolder->setup_dependencies(m_pADIC);
    ASSERT_FALSE(testFolder->is_valid());
}
TEST_F(folderInvalid, playModeUndefined_isValidReturnsFalse)
{
    Folder testFolder(1, Folder::UNDEFINED, 24);
    testFolder->setup_dependencies(m_pADIC);
    ASSERT_FALSE(testFolder->is_valid());
}
TEST_F(folderInvalid, folderIdIs0_isValidReturnsFalse)
{
    Folder testFolder(0, Folder::ALBUM, 24);
    testFolder->setup_dependencies(m_pADIC);
    ASSERT_FALSE(testFolder->is_valid());
}
TEST_F(folderMethods, folder_ALBUM_valid)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(m_pADIC);
    ASSERT_TRUE(testFolder.is_valid());
}
TEST_F(folderMethods, copyConstructor_workingOK)
{
    // Arrange
    EXPECT_CALL(m_pEeprom, eeprom_read(254)).WillOnce(Return(13));
    Folder testFolder(254, Folder::SAVEPROGRESS, 16);
    testFolder.setup_dependencies(m_pADIC);
    // Act
    Folder copyFolder(testFolder);
    // Assert
    EXPECT_TRUE(copyFolder.is_valid());
}

TEST_F(folderMethods, copyConstructor_valuesOK)
{
    EXPECT_CALL(m_pEeprom, eeprom_read(254)).WillOnce(Return(13));
    Folder testFolder(54, Folder::SAVEPROGRESS, 16);
    testFolder.setup_dependencies(m_pADIC);

    Folder copyFolder(testFolder);

    // Validate that contents is copied OK
    EXPECT_EQ(13, copyFolder.get_current_track());
    EXPECT_EQ(254, copyFolder.get_folder_id());
    EXPECT_EQ(Folder::SAVEPROGRESS, copyFolder.get_play_mode());
    EXPECT_EQ(16, copyFolder.get_track_count());
}

TEST_F(folderMethods, assignmentOperator_workingOK)
{
    Folder testFolder(254, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&m_pADIC);
    Folder copyFolder = Folder();

    copyFolder = testFolder;
    // Check if variables are set OK
    EXPECT_TRUE(copyFolder.is_valid());
}

TEST_F(folderMethods, assignmentOperator_valuesOK)
{
    Folder testFolder(254, Folder::ALBUM, 10);
    testFolder->setup_dependencies(&m_pADIC);
    Folder copyFolder = Folder();

    copyFolder = testFolder;

    // Validate that contents is copied OK
    EXPECT_EQ(1, copyFolder.get_current_track());
    EXPECT_EQ(254, copyFolder.get_folder_id());
    EXPECT_EQ(Folder::ALBUM, copyFolder.get_play_mode());
    EXPECT_EQ(10, copyFolder.get_track_count());
}

TEST_F(folderAlbum, ALBUM_idIs2)
{
    ASSERT_EQ(2, testFolder->get_folder_id());
}

TEST_F(folderAlbum, playModeIsALBUM)
{
    ASSERT_EQ(Folder::ALBUM, testFolder->get_play_mode());
}
TEST_F(folderAlbum, trackCountIs10)
{
    ASSERT_EQ(10, testFolder->get_track_count());
}
TEST_F(folderAlbum, currentTrackIs1)
{
    ASSERT_EQ(1, testFolder->get_current_track());
}
TEST_F(folderAlbum, nextTrackIs2)
{
    ASSERT_EQ(2, testFolder->get_next_track());
}
TEST_F(folderAlbum, ChooseNextTrack_currentTrackIs2)
{
    testFolder->get_next_track();
    ASSERT_EQ(2, testFolder->get_current_track());
}
TEST_F(folderAlbum, trackCount10_CallPrevTrack_returns10)
{
    // Tests rollover track count
    ASSERT_EQ(10, testFolder->get_prev_track());
}
TEST_F(folderAlbum, trackCount10_CallPrevTrack_GetCurrentTrackReturns10)
{
    // Tests rollover track count
    ASSERT_EQ(10, testFolder->get_current_track());
}
TEST_F(folderAlbum, trackCountIs10_AllTracksAreInQueue)
{
    uint8_t trackSum = testFolder->get_current_track();
    for (int i = 2; i <= testFolder->get_track_count(); ++i)
    {
        trackSum += testFolder->get_next_track();
    }
    ASSERT_EQ(21, trackSum);
}

TEST_F(folderDependencies, RANDOM_trackCountIs10_trackQueueLoopComplete)
{
    Folder testFolder(1, Folder::RANDOM, 10);
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
        .WillOnce(Return(6))
    testFolder->setup_dependencies(m_pADIC);
    // take sample that track queue is correctly setup
    // ALSO CONFIRMS THAT TRACK LIST IS OK BECAUSE OF WILLONCE STRUCTURE
    EXPECT_EQ(1, testFolder.get_current_track());
    EXPECT_EQ(10, testFolder.get_next_track());
}

TEST_F(folderDependencies, RANDOM_trackCountIs10_willNotAllowSameTrackMultipleTimesInQueue)
{
    Folder testFolder(1, Folder::RANDOM, 10);
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
        .WillOnce(Return(6))

    testFolder->setup_dependencies(m_pADIC);
    // take sample that track queue is correctly setup
    testFolder.get_next_track();
    ASSERT_EQ(2, testFolder.get_next_track());
}


TEST_F(folderDependencies, RANDOM_trackCountIs10_willNotAllowTrackNumber0)
{
    Folder testFolder(1, Folder::RANDOM, 10);
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
        .WillOnce(Return(6))

    testFolder->setup_dependencies(m_pADIC);
    // take sample that track queue is correctly setup
    ASSERT_EQ(10, testFolder.get_next_track());
}

TEST_F(folderDependencies, RANDOM_trackCountIs10_willNotAllowTrackNumberOutOfRange)
{
    Folder testFolder(1, Folder::RANDOM, 10);
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
        .WillOnce(Return(6))

    testFolder->setup_dependencies(m_pADIC);
    // take sample that track queue is correctly setup
    ASSERT_EQ(10, testFolder.get_next_track());
}

TEST_F(folderDependencies, SAVEPROGRESS_trackLoadIsWorking)
{
    Folder testFolder(99, Folder::SAVEPROGRESS, 16);

    ON_CALL(*m_pEeprom, eeprom_read(_)).WillByDefault(Return(13));
    testFolder->setup_dependencies(m_pADIC);

    EXPECT_EQ(13, testFolder->get_current_track());
}

TEST_F(folderDependencies, SAVEPROGRESS_trackSaveIsWorking)
{
    Folder testFolder(99, Folder::SAVEPROGRESS, 16);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WillByDefault(Return(13));

    EXPECT_CALL(*m_pEeprom, eeprom_write(254, 14));
    testFolder->setup_dependencies(m_pADIC);
    EXPECT_EQ(14, testFolder->get_next_track());
}

TEST_F(folderDependencies, SAVEPROGRESS_EepromRead0_currentTrackDefaultsTo1)
{
    Folder testFolder(99, Folder::SAVEPROGRESS, 16);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WilByDefault(Return(0));
    
    testFolder->setup_dependencies(m_pADIC);
    EXPECT_EQ(1, testFolder->get_current_track());
}
TEST_F(folderDependencies, SAVEPROGRESS_EepromReadOutOfRange_currentTrackDefaultsTo1)
{
    Folder testFolder(99, Folder::SAVEPROGRESS, 16);
    ON_CALL(*m_pEeprom, eeprom_read(_)).WilByDefault(Return(17));
    
    testFolder->setup_dependencies(m_pADIC);
    EXPECT_EQ(1, testFolder->get_current_track());
}