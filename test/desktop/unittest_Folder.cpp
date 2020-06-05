#include "unittest_Folder.h"

void folder_invalid(void)
{
    Folder testFolder;
    TEST_ASSERT_EQUAL(false, testFolder.is_valid());
}
void folder_invalid_idIs0(void)
{
    Folder testFolder;
    TEST_ASSERT_EQUAL(0, testFolder.get_folder_id());
}
void folder_invalid_playModeIsUndefined(void)
{
    Folder testFolder;
    TEST_ASSERT_EQUAL(Folder::UNDEFINED, testFolder.get_play_mode());
}
void folder_invalid_trackCountIs0(void)
{
    Folder testFolder;
    TEST_ASSERT_EQUAL(0, testFolder.get_track_count());
}
void folder_invalid_currentTrackIs0(void)
{
    Folder testFolder;
    TEST_ASSERT_EQUAL(0, testFolder.get_current_track());
}
void folder_invalid_nextTrackIs0(void)
{
    Folder testFolder;
    TEST_ASSERT_EQUAL(0, testFolder.get_next_track());
}
void folder_invalid_prevTrackIs0(void)
{
    Folder testFolder;
    TEST_ASSERT_EQUAL(0, testFolder.get_prev_track());
}
void folder_invalid_trackCount0_isValidReturnsFalse(void)
{
    Folder testFolder(1, Folder::ALBUM, 0);
    TEST_ASSERT_EQUAL(false, testFolder.is_valid());
}
void folder_invalid_playModeUndefined_isValidReturnsFalse(void)
{
    Folder testFolder(1, Folder::UNDEFINED, 24);
    TEST_ASSERT_EQUAL(false, testFolder.is_valid());
}
void folder_invalid_folderIdIs0_isValidReturnsFalse(void)
{
    Folder testFolder(0, Folder::ALBUM, 24);
    TEST_ASSERT_EQUAL(false, testFolder.is_valid());
}
void folder_ALBUM_valid(void)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    TEST_ASSERT_EQUAL(true, testFolder.is_valid());
}
void folder_ALBUM_idIs2(void)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    TEST_ASSERT_EQUAL(2, testFolder.get_folder_id());
}
void folder_ALBUM_playModeIsALBUM(void)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    TEST_ASSERT_EQUAL(Folder::ALBUM, testFolder.get_play_mode());
}
void folder_ALBUM_trackCountIs10(void)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    TEST_ASSERT_EQUAL(10, testFolder.get_track_count());
}
void folder_ALBUM_trackCount10_currentTrackIs1(void)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    TEST_ASSERT_EQUAL(1, testFolder.get_current_track());
}
void folder_ALBUM_trackCountIs10_nextTrackIs2(void)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    TEST_ASSERT_EQUAL(2, testFolder.get_next_track());
    TEST_ASSERT_EQUAL(2, testFolder.get_current_track());
}
void folder_ALBUM_trackCountIs10_prevTrackIs10(void)
{
    Folder testFolder(2, Folder::ALBUM, 10);
    TEST_ASSERT_EQUAL(10, testFolder.get_prev_track());
    TEST_ASSERT_EQUAL(10, testFolder.get_current_track());
}
void folder_ALBUM_trackCountIs6_AllTracksAreInQueue(void)
{
    Folder testFolder(1, Folder::ALBUM, 6);
    uint8_t trackSum = testFolder.get_current_track();
    for (int i = 2; i <= 6; ++i)
    {
        trackSum += testFolder.get_next_track();
    }
    TEST_ASSERT_EQUAL(21, trackSum);
}
void folder_RANDOM_trackCountIs10_NextTracksNotConsecutiveOrNull(void)
{
    Folder testFolder(1, Folder::RANDOM, 10);
    uint8_t track1 = testFolder.get_current_track();
    uint8_t track2 = testFolder.get_next_track();
    uint8_t track3 = testFolder.get_next_track();
    uint8_t track4 = testFolder.get_next_track();
    bool tracksConsecutive = ((track4 - track3) == 1) && ((track3 - track2) == 1) && ((track2 - track1) == 1);
    bool tracksNull = (track1 == 0 || track2 == 0 || track3 == 0 || track4 == 0);
    TEST_ASSERT_EQUAL(false, tracksConsecutive);
    TEST_ASSERT_EQUAL(false, tracksNull);
}
void folder_RANDOM_trackCountIs6_AllTracksAreInQueue(void)
{
    Folder testFolder(1, Folder::RANDOM, 6);
    uint8_t trackSum = testFolder.get_current_track();
    for (int i = 2; i <= 6; ++i)
    {
        trackSum += testFolder.get_next_track();
    }
    TEST_ASSERT_EQUAL(21, trackSum);
}
void folder_SAVEPROGRESS_trackSaveAndLoad_isWorking(void)
{
    Folder testFolder(254, Folder::SAVEPROGRESS, 4);
    TEST_ASSERT_EQUAL(1, testFolder.get_current_track());
    testFolder.get_next_track();
    testFolder = Folder(254, Folder::SAVEPROGRESS, 4);
    TEST_ASSERT_EQUAL(2, testFolder.get_current_track());
    testFolder.get_prev_track(); // Revert to 1
}

void unittests()
{
    RUN_TEST(folder_invalid);
    RUN_TEST(folder_invalid_idIs0);
    RUN_TEST(folder_invalid_playModeIsUndefined);
    RUN_TEST(folder_invalid_trackCountIs0);
    RUN_TEST(folder_invalid_currentTrackIs0);
    RUN_TEST(folder_invalid_nextTrackIs0);
    RUN_TEST(folder_invalid_prevTrackIs0);
    RUN_TEST(folder_invalid_trackCount0_isValidReturnsFalse);
    RUN_TEST(folder_invalid_playModeUndefined_isValidReturnsFalse);
    RUN_TEST(folder_invalid_folderIdIs0_isValidReturnsFalse);
    RUN_TEST(folder_ALBUM_valid);
    RUN_TEST(folder_ALBUM_idIs2);
    RUN_TEST(folder_ALBUM_playModeIsALBUM);
    RUN_TEST(folder_ALBUM_trackCountIs10);
    RUN_TEST(folder_ALBUM_trackCount10_currentTrackIs1);
    RUN_TEST(folder_ALBUM_trackCountIs10_nextTrackIs2);
    RUN_TEST(folder_ALBUM_trackCountIs10_prevTrackIs10);
    RUN_TEST(folder_ALBUM_trackCountIs6_AllTracksAreInQueue);
    RUN_TEST(folder_RANDOM_trackCountIs10_NextTracksNotConsecutiveOrNull);
    RUN_TEST(folder_RANDOM_trackCountIs6_AllTracksAreInQueue);
    RUN_TEST(folder_SAVEPROGRESS_trackSaveAndLoad_isWorking);
}