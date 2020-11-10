#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_PromptPlayer_mocks.h"

#include "Menu_implementation.h"

class LinkMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        linkMenu = Menu_factory::getInstance(Menu_factory::LINK_MENU,
                                             &m_nfcControlMock,
                                             &m_promptPlayerMock);
    }

    virtual void TearDown()
    {
        delete linkMenu;
    }

protected:
    Menu_interface *linkMenu{nullptr};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_PromptPlayer> m_promptPlayerMock{};
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

/*


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
    EXPECT_EQ(result.promptId, 0);
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

TEST_F(LinkMenuTest, noInit_getPreview_returnsInvalidFolder)
{
    Folder testFolder = linkMenu->getPreview();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, selectFolderId_noSelection_getPreview_returnsInvalidFolder)
{
    linkMenu->confirm();

    Folder testFolder = linkMenu->getPreview();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, selectPlayMode_noSelection_getPreview_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getPreview();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, confirm3x_getPreview_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getPreview();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, selectFolderId1_getPreview_returnsPreviewFolder1)
{
    linkMenu->confirm();
    linkMenu->selectNext();

    Folder testFolder = linkMenu->getPreview();

    ASSERT_EQ(testFolder.get_folder_id(), 1);
    ASSERT_EQ(testFolder.get_play_mode(), Folder::ONELARGETRACK);
    ASSERT_EQ(testFolder.get_track_count(), 1);
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPreview_returnsPreviewFolderMAX)
{
    linkMenu->confirm();
    linkMenu->selectPrev();

    Folder testFolder = linkMenu->getPreview();

    ASSERT_EQ(testFolder.get_folder_id(), MAXFOLDERCOUNT);
    ASSERT_EQ(testFolder.get_play_mode(), Folder::ONELARGETRACK);
    ASSERT_EQ(testFolder.get_track_count(), 1);
}

TEST_F(LinkMenuTest, selectFolderId1_confirm_getPreview_returnsUndefinedPlaymode)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getPreview();

    ASSERT_EQ(testFolder.get_folder_id(), 1);
    ASSERT_EQ(testFolder.get_play_mode(), Folder::UNDEFINED);
    ASSERT_EQ(testFolder.get_track_count(), 1);
}

TEST_F(LinkMenuTest, selectFolderIdAndPlayMode_getPreview_returnsInvalidFolder)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();

    Folder testFolder = linkMenu->getPreview();

    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, confirmFolderIdAndPlayMode_getPreview_returnsValidFolder)
{
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    Folder testFolder = linkMenu->getPreview();

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

TEST_F(LinkMenuTest, noInit_abort_getPreview_returnsInvalidFolder)
{
    linkMenu->abort();

    Folder testFolder = linkMenu->getPreview();
    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, folderIdSet_abort_getPreview_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    linkMenu->abort();

    Folder testFolder = linkMenu->getPreview();
    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, menuComplete_abort_getPreview_returnsInvalidFolder)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    linkMenu->abort();

    Folder testFolder = linkMenu->getPreview();
    ASSERT_FALSE(testFolder.is_valid());
}

TEST_F(LinkMenuTest, noInit_isPeviewAvailable_returnsFalse)
{
    ASSERT_FALSE(linkMenu->isPreviewAvailable());
}

TEST_F(LinkMenuTest, folderIdSelect_isPeviewAvailable_returnsTrue)
{
    linkMenu->confirm();
    linkMenu->selectNext();

    ASSERT_TRUE(linkMenu->isPreviewAvailable());
}

TEST_F(LinkMenuTest, playModeSelect_isPeviewAvailable_returnsFalse)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    ASSERT_FALSE(linkMenu->isPreviewAvailable());
}

TEST_F(LinkMenuTest, menuComplete_isPeviewAvailable_returnsFalse)
{
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    ASSERT_FALSE(linkMenu->isPreviewAvailable());
}
*/