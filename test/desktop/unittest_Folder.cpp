#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Folder.h>
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"

using ::testing::Return;    
TEST(folder, folder_invalid)
{
    Folder testFolder;
    ASSERT_EQ(false, testFolder.is_valid());
}
TEST(folder, folder_invalid_idIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder.get_folder_id());
}
TEST(folder, folder_invalid_playModeIsUndefined)
{
    Folder testFolder;
    ASSERT_EQ(Folder::UNDEFINED, testFolder.get_play_mode());
}
TEST(folder, folder_invalid_trackCountIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder.get_track_count());
}
TEST(folder, folder_invalid_currentTrackIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder.get_current_track());
}
TEST(folder, folder_invalid_nextTrackIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder.get_next_track());
}
TEST(folder, folder_invalid_prevTrackIs0)
{
    Folder testFolder;
    ASSERT_EQ(0, testFolder.get_prev_track());
}
TEST(folder, folder_invalid_trackCount0_isValidReturnsFalse)
{
    
    Folder testFolder(1, Folder::ALBUM, 0);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_FALSE(testFolder.is_valid());
}
TEST(folder, folder_invalid_playModeUndefined_isValidReturnsFalse)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(1, Folder::UNDEFINED, 24);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_FALSE(testFolder.is_valid());
}
TEST(folder, folder_invalid_folderIdIs0_isValidReturnsFalse)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(0, Folder::ALBUM, 24);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_FALSE(testFolder.is_valid());
}
TEST(folder, folder_ALBUM_valid)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_TRUE(testFolder.is_valid());
}
TEST(folder, folder_copyConstructor_workingOK)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Mock_eeprom eeprom;
    ON_CALL(arduDIcontainer, getEeprom()).WillByDefault(Return(&eeprom));
    EXPECT_CALL(eeprom, read(254)).WillOnce(Return(13));
    Folder testFolder(254, Folder::SAVEPROGRESS, 16);
    testFolder.setup_dependencies(&arduDIcontainer);
    Folder copyFolder(testFolder);
    // Check if variables are set OK
    EXPECT_TRUE(copyFolder.is_valid());
    // Validate that contents is copied OK
    EXPECT_EQ(13, copyFolder.get_current_track());
    EXPECT_EQ(254, copyFolder.get_folder_id());
    EXPECT_EQ(Folder::SAVEPROGRESS, copyFolder.get_play_mode());
    EXPECT_EQ(16, copyFolder.get_track_count());
}
TEST(folder, folder_assignmentOperator_workingOK)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(254, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    Folder copyFolder = Folder();
    copyFolder = testFolder;
    // Check if variables are set OK
    EXPECT_TRUE(copyFolder.is_valid());
    // Validate that contents is copied OK
    EXPECT_EQ(1, copyFolder.get_current_track());
    EXPECT_EQ(254, copyFolder.get_folder_id());
    EXPECT_EQ(Folder::ALBUM, copyFolder.get_play_mode());
    EXPECT_EQ(10, copyFolder.get_track_count());
} 
TEST(folder, folder_ALBUM_idIs2)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_EQ(2, testFolder.get_folder_id());
}
TEST(folder, folder_ALBUM_playModeIsALBUM)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_EQ(Folder::ALBUM, testFolder.get_play_mode());
}
TEST(folder, folder_ALBUM_trackCountIs10)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_EQ(10, testFolder.get_track_count());
}
TEST(folder, folder_ALBUM_trackCount10_currentTrackIs1)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    ASSERT_EQ(1, testFolder.get_current_track());
}
TEST(folder, folder_ALBUM_trackCountIs10_nextTrackIs2)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    EXPECT_EQ(1, testFolder.get_current_track());
    EXPECT_EQ(2, testFolder.get_next_track());
    EXPECT_EQ(2, testFolder.get_current_track());
}
TEST(folder, folder_ALBUM_trackCountIs10_prevTrackIs10)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(2, Folder::ALBUM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    // Tests rollover track count
    EXPECT_EQ(1, testFolder.get_current_track());
    EXPECT_EQ(10, testFolder.get_prev_track());
    EXPECT_EQ(10, testFolder.get_current_track());
}
TEST(folder, folder_ALBUM_trackCountIs6_AllTracksAreInQueue)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(1, Folder::ALBUM, 6);
    testFolder.setup_dependencies(&arduDIcontainer);
    uint8_t trackSum = testFolder.get_current_track();
    for (int i = 2; i <= 6; ++i)
    {
        trackSum += testFolder.get_next_track();
    }
    ASSERT_EQ(21, trackSum);
}
TEST(folder, folder_RANDOM_trackCountIs10_NextTracksNotConsecutiveOrNull)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(1, Folder::RANDOM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    uint8_t track1 = testFolder.get_current_track();
    uint8_t track2 = testFolder.get_next_track();
    uint8_t track3 = testFolder.get_next_track();
    uint8_t track4 = testFolder.get_next_track();
    bool tracksConsecutive = (bool)(((track4 - track3) == 1) && ((track3 - track2) == 1) && ((track2 - track1) == 1));
    bool tracksNull = (track1 == 0 || track2 == 0 || track3 == 0 || track4 == 0);
    EXPECT_FALSE(tracksConsecutive);
    EXPECT_FALSE(tracksNull);
}
TEST(folder, folder_RANDOM_invalidSeed_NextTracksNull)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(1, Folder::RANDOM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    uint8_t track1 = testFolder.get_current_track();
    uint8_t track2 = testFolder.get_next_track();
    uint8_t track3 = testFolder.get_next_track();
    uint8_t track4 = testFolder.get_next_track();
    bool tracksConsecutive = (bool)(((track4 - track3) == 1) && ((track3 - track2) == 1) && ((track2 - track1) == 1));
    bool tracksNull = (track1 == 0 || track2 == 0 || track3 == 0 || track4 == 0);
    EXPECT_FALSE(tracksConsecutive);
    EXPECT_TRUE(tracksNull); // Without a random seed, TrackQueue will not be setup. isValid = FALSE.
}
TEST(folder, folder_RANDOM_trackCountIs6_AllTracksAreInQueue)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Folder testFolder(1, Folder::RANDOM, 10);
    testFolder.setup_dependencies(&arduDIcontainer);
    uint8_t trackSum = testFolder.get_current_track();
    for (int i = 2; i <= 10; ++i)
    {
        trackSum += testFolder.get_next_track();
    }
    ASSERT_EQ(55, trackSum);
}
TEST(folder, folder_SAVEPROGRESS_trackLoad_isWorking)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Mock_eeprom eeprom;
    ON_CALL(arduDIcontainer, getEeprom()).WillByDefault(Return(&eeprom));
    EXPECT_CALL(eeprom, read(254)).WillOnce(Return(13));
    Folder testFolder(254, Folder::SAVEPROGRESS, 16);
    testFolder.setup_dependencies(&arduDIcontainer);
    EXPECT_EQ(13, testFolder.get_current_track());
}
TEST(folder, folder_SAVEPROGRESS_trackSave_isWorking)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Mock_eeprom eeprom;
    ON_CALL(arduDIcontainer, getEeprom()).WillByDefault(Return(&eeprom));
    EXPECT_CALL(eeprom, write(254, 14));
    EXPECT_CALL(eeprom, read(254)).WillOnce(Return(13));
    Folder testFolder(254, Folder::SAVEPROGRESS, 16);
    testFolder.setup_dependencies(&arduDIcontainer);
    EXPECT_EQ(14, testFolder.get_next_track());
}
TEST(folder, folder_SAVEPROGRESS_Eeprom0_DefaultsTo1)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Mock_eeprom eeprom;
    ON_CALL(arduDIcontainer, getEeprom()).WillByDefault(Return(&eeprom));
    EXPECT_CALL(eeprom, write(254,1));
    EXPECT_CALL(eeprom, read(254)).WillOnce(Return(0));
    Folder testFolder(254, Folder::SAVEPROGRESS, 16);
    testFolder.setup_dependencies(&arduDIcontainer);
    EXPECT_EQ(1, testFolder.get_current_track());
}
TEST(folder, folder_SAVEPROGRESS_EepromLargerThanTrackCount_DefaultsTo1)
{
    Mock_ArduinoDIcontainer arduDIcontainer;
    Mock_eeprom eeprom;
    ON_CALL(arduDIcontainer, getEeprom()).WillByDefault(Return(&eeprom));
    EXPECT_CALL(eeprom, write(14,1));
    EXPECT_CALL(eeprom, read(14)).WillOnce(Return(17));
    Folder testFolder(14, Folder::SAVEPROGRESS, 16);
    testFolder.setup_dependencies(&arduDIcontainer);
    EXPECT_EQ(1, testFolder.get_current_track());
}