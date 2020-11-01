#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Menu_implementation.h"

class LinkMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        linkMenu = Menu_factory::getInstance(Menu_factory::LINK_MENU);
    }

    virtual void TearDown()
    {
        delete linkMenu;
    }

protected:
    Menu_interface *linkMenu{nullptr};
};

// INIT() ------------------------------------------------------
TEST_F(LinkMenuTest, noInit_isActive_returnsFalse)
{
    ASSERT_FALSE(linkMenu->isActive());
}

TEST_F(LinkMenuTest, entered_isActive_returnsTrue)
{
    linkMenu->confirm();

    ASSERT_TRUE(linkMenu->isActive());
}

TEST_F(LinkMenuTest, entered_abort_isActive_returnsFalse)
{
    linkMenu->confirm();
    linkMenu->abort();

    ASSERT_FALSE(linkMenu->isActive());
}





TEST_F(LinkMenuTest, noInit_noPromptSet)
{
    ASSERT_EQ((linkMenu->getPrompt()).promptId, 0);
}

TEST_F(LinkMenuTest, selectFolderId_noSelection_getPrompt_FolderIdAndAllowSkip)
{
    linkMenu->confirm();
    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MSG_SELECT_FOLDERID);
    EXPECT_EQ(result.allowSkip, true);
}

TEST_F(LinkMenuTest, selectPlayMode_noSelection_getPrompt_PlayModeAndAllowSkip)
{
    linkMenu->confirm();
    linkMenu->confirm();
    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MSG_SELECT_PLAYMODE);
    EXPECT_EQ(result.allowSkip, true);
}

TEST_F(LinkMenuTest, menuComplete_noSelection_getPrompt_TagConfigSuccess)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();
    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MSG_TAGCONFSUCCESS);
    EXPECT_EQ(result.allowSkip, true);
}

TEST_F(LinkMenuTest, menuComplete_noSelection_confirmAgainAndGetPrompt_TagConfigSuccess)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();
    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MSG_TAGCONFSUCCESS);
    EXPECT_EQ(result.allowSkip, true);
}

TEST_F(LinkMenuTest, selectFolderId1_getPrompt_returns1)
{
    linkMenu->confirm();
    linkMenu->selectNext();

    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, 1);
    EXPECT_EQ(result.allowSkip, false);
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPrompt_returnsMAX)
{
    linkMenu->confirm();
    linkMenu->selectPrev();

    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MAXFOLDERCOUNT);
    EXPECT_EQ(result.allowSkip, false);
}

TEST_F(LinkMenuTest, selectPlayMode1_getPrompt_returnsLULLABYE)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectNext();

    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MSG_SELECT_PLAYMODE + static_cast<uint16_t>(Folder::LULLABYE));
    EXPECT_EQ(result.allowSkip, false);
}

TEST_F(LinkMenuTest, selectPlayModeMAX_getPrompt_returnsONELARGETRACK)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectPrev();

    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MSG_SELECT_PLAYMODE + static_cast<uint16_t>(Folder::ONELARGETRACK));
    EXPECT_EQ(result.allowSkip, false);
}

TEST_F(LinkMenuTest, selectPlayMode_testRollover_getPrompt_returnsONELARGETRACK)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectPrev();
    linkMenu->selectPrev();

    VoicePrompt result = linkMenu->getPrompt();
    EXPECT_EQ(result.promptId, MSG_SELECT_PLAYMODE + static_cast<uint16_t>(Folder::SAVEPROGRESS));
    EXPECT_EQ(result.allowSkip, false);
}

TEST_F(LinkMenuTest, noInit_getFolderInformation_returnsInvalidFolder)
{
    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, selectFolderId_noSelection_getFolderInformation_returnsInvalidFolder)
{
    linkMenu->confirm();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, selectPlayMode_noSelection_getFolderInformation_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, confirm3x_getFolderInformation_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, selectFolderId1_getFolderInformation_returnsPreviewFolder1)
{
    linkMenu->confirm();
    linkMenu->selectNext();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_EQ(testFolder.get_folder_id(), 1);
    ASSERT_EQ(testFolder.get_play_mode(), Folder::ONELARGETRACK);
    ASSERT_EQ(testFolder.get_track_count(), 1);
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getFolderInformation_returnsPreviewFolderMAX)
{
    linkMenu->confirm();
    linkMenu->selectPrev();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_EQ(testFolder.get_folder_id(), MAXFOLDERCOUNT);
    ASSERT_EQ(testFolder.get_play_mode(), Folder::ONELARGETRACK);
    ASSERT_EQ(testFolder.get_track_count(), 1);
}

TEST_F(LinkMenuTest, selectFolderId1_confirm_getFolderInformation_returnsUndefinedPlaymode)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_EQ(testFolder.get_folder_id(), 1);
    ASSERT_EQ(testFolder.get_play_mode(), Folder::UNDEFINED);
    ASSERT_EQ(testFolder.get_track_count(), 1);
}

TEST_F(LinkMenuTest, selectFolderIdAndPlayMode_getFolderInformation_returnsInvalidFolder)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, confirmFolderIdAndPlayMode_isComplete_returnsTrue)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    ASSERT_TRUE(linkMenu->isComplete());
}

TEST_F(LinkMenuTest, confirmFolderIdAndPlayMode_isActive_returnsTrue)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    ASSERT_TRUE(linkMenu->isActive());
}

TEST_F(LinkMenuTest, confirmFolderIdAndPlayMode_getFolderInformation_returnsValidFolder)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getFolderInformation();

    ASSERT_TRUE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, noInit_abort_noPromptSet)
{
    linkMenu->abort();

    ASSERT_EQ((linkMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(LinkMenuTest, folderId1Selected_abort_promptsAborted)
{
    linkMenu->confirm();
    linkMenu->selectNext();

    linkMenu->abort();

    ASSERT_EQ((linkMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(LinkMenuTest, folderIdAndPlayModeSelected_abort_promptsAborted)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();

    linkMenu->abort();

    ASSERT_EQ((linkMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(LinkMenuTest, menuCompleted_abort_promptsAborted)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    linkMenu->abort();

    ASSERT_EQ((linkMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(LinkMenuTest, noInit_abort_getFolderInformation_returnsInvalidFolder)
{
    linkMenu->abort();

    Folder testFolder = linkMenu->getFolderInformation();
    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, folderIdSet_abort_getFolderInformation_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    linkMenu->abort();

    Folder testFolder = linkMenu->getFolderInformation();
    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, menuComplete_abort_getFolderInformation_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    linkMenu->abort();

    Folder testFolder = linkMenu->getFolderInformation();
    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, noInit_lockInLinkMenu_notLocked)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NO_TAG;
        
    linkMenu->updateTagState(tagState);

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

TEST_F(LinkMenuTest, entered_lockInLinkMenu_locked)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NO_TAG;
        
    linkMenu->confirm();
    linkMenu->updateTagState(tagState);

    ASSERT_EQ(tagState, Nfc_interface::NEW_UNKNOWN_TAG);
}

TEST_F(LinkMenuTest, menuComplete_lockInLinkMenu_unLocked)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NO_TAG;
        
    linkMenu->confirm(); // enter
    linkMenu->confirm(); // folderId selected
    linkMenu->confirm(); // playMode selected: complete
    
    linkMenu->updateTagState(tagState);

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

TEST_F(LinkMenuTest, menuAbort_lockInLinkMenu_unLocked)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NO_TAG;
        
    linkMenu->confirm(); // enter
    linkMenu->confirm(); // folderId selected
    linkMenu->abort();
    
    linkMenu->updateTagState(tagState);

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

