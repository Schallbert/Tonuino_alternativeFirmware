#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"

#include "Menu_factory.h"

class LinkMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        linkMenu = Menu_factory::getInstance(Menu_factory::LINK_MENU,
                                             &m_nfcControlMock,
                                             &m_Mp3PlayMock,
                                             &m_powerManagerMock);
    }

    virtual void TearDown()
    {
        delete linkMenu;
    }

protected:
    Menu_interface *linkMenu{nullptr};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_Mp3Play> m_Mp3PlayMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
};

MATCHER(invalidPrompt, "")
{
    return (arg.promptId == 0);
}

MATCHER_P(identicalPrompt, comp, "")
{
    return (
        (arg.promptId == comp.promptId) &&
        (arg.allowSkip == comp.allowSkip));
}

MATCHER(invalidFolder, "")
{
    return (!arg.isInitiated());
}

MATCHER(validFolder, "")
{
    return (arg.isInitiated());
}

MATCHER_P(identicalFolder, comp, "")
{
    return ((arg.getFolderId() == comp.getFolderId()) &&
            (arg.getPlayMode() == comp.getPlayMode()) &&
            (arg.getTrackCount() == comp.getTrackCount()));
}

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

TEST_F(LinkMenuTest, noInit_setStatusLed_noStatusLedChangeRequested)
{
    EXPECT_CALL(m_powerManagerMock, setLinkMenu()).Times(0);
    linkMenu->setStatusLed();
}

TEST_F(LinkMenuTest, entered_setStatusLed_statusLedSetTolinkMenu)
{
    linkMenu->confirm();

    EXPECT_CALL(m_powerManagerMock, setLinkMenu());
    linkMenu->setStatusLed();
}

TEST_F(LinkMenuTest, menuComplete_setStatusLed_statusLedChangeRequested)
{
    linkMenu->confirm(); // enter
    linkMenu->confirm(); // confirm folderId
    linkMenu->confirm(); // confirm playMode -->complete

    EXPECT_CALL(m_powerManagerMock, setLinkMenu());
    linkMenu->setStatusLed();
}

TEST_F(LinkMenuTest, menuAbort_setStatusLed_noStatusLedChangeRequested)
{
    linkMenu->confirm();   
    linkMenu->abort();      

    EXPECT_CALL(m_powerManagerMock, setLinkMenu()).Times(0);
    linkMenu->setStatusLed();
}

// Prompt tests
TEST_F(LinkMenuTest, noInit_noPromptSet)
{
    EXPECT_CALL(m_Mp3PlayMock, playPrompt(invalidPrompt()));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderId_noSelection_getPrompt_FolderIdAndAllowSkip)
{
    VoicePrompt expect{};
    expect.promptId = MSG_SELECT_FOLDERID;
    expect.allowSkip = true;

    linkMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayMode_noSelection_getPrompt_PlayModeAndAllowSkip)
{
    VoicePrompt expect{};
    expect.promptId = MSG_SELECT_PLAYMODE;
    expect.allowSkip = true;

    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, menuComplete_noSelection_getPrompt_TagConfigSuccess)
{
    VoicePrompt expect{};
    expect.promptId = MSG_TAGCONFSUCCESS;
    expect.allowSkip = true;

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, menuComplete_noSelection_confirmAgainAndGetPrompt_noPromptSet)
{
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(invalidPrompt()));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderId1_getPrompt_returns1)
{
    VoicePrompt expect{};
    expect.promptId = 1;
    expect.allowSkip = false;

    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPrompt_returnsMAX)
{
    VoicePrompt expect{};
    expect.promptId = MAXFOLDERCOUNT;
    expect.allowSkip = false;

    linkMenu->confirm();
    linkMenu->selectPrev();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayMode1_getPrompt_returnsLULLABYE)
{
    VoicePrompt expect{};
    expect.promptId = MSG_SELECT_PLAYMODE + static_cast<uint16_t>(Folder::LULLABYE);
    expect.allowSkip = false;

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayModeMAX_getPrompt_returnsONELARGETRACK)
{
    VoicePrompt expect{};
    expect.promptId = MSG_SELECT_PLAYMODE + static_cast<uint16_t>(Folder::ONELARGETRACK);
    expect.allowSkip = false;

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectPrev();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayMode_testRollover_getPrompt_returnsSAVEPROGRESS)
{
    VoicePrompt expect{};
    expect.promptId = MSG_SELECT_PLAYMODE + static_cast<uint16_t>(Folder::SAVEPROGRESS);
    expect.allowSkip = false;

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectPrev();
    linkMenu->selectPrev();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, noInit_abort_noPromptSet)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    linkMenu->abort();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, folderId1Selected_abort_promptsAborted)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    linkMenu->confirm();
    linkMenu->selectNext();

    linkMenu->abort();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, folderIdAndPlayModeSelected_abort_promptsAborted)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();

    linkMenu->abort();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, menuCompleted_abort_promptsAborted)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    linkMenu->abort();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

// Folder Preview Tests
TEST_F(LinkMenuTest, selectFolderId1_getPreview_returnsPreviewFolder1)
{
    Folder expect{Folder(1, Folder::ONELARGETRACK)};

    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_Mp3PlayMock, playFolder(identicalFolder(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPreview_returnsPreviewFolderMAX)
{
    Folder expect{Folder(MAXFOLDERCOUNT, Folder::ONELARGETRACK)};

    linkMenu->confirm();
    linkMenu->selectPrev();

    EXPECT_CALL(m_Mp3PlayMock, playFolder(identicalFolder(expect)));
    linkMenu->handlePlayback();
}