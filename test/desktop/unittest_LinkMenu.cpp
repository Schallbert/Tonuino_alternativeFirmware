#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"
#include "mocks/unittest_VoiceMenu.h"

#include "Tonuino_config.h"
#include "Folder.h"

#include "Menu_factory.h"

class LinkMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        linkMenu = m_MenuFactory.getInstance(Menu_factory::LINK_MENU,
                                             m_nfcControlMock,
                                             m_mp3PlayMock,
                                             m_messageHandlerMock,
                                             m_powerManagerMock);
    }

protected:
    Menu_interface *linkMenu{nullptr};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};

    Menu_factory m_MenuFactory;
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
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(true));
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    ASSERT_TRUE(linkMenu->isComplete());
}

TEST_F(LinkMenuTest, menuComplete_writesConfigToCard)
{
    EXPECT_CALL(m_nfcControlMock, writeFolderToTag(_)).WillOnce(Return(true));
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    ASSERT_TRUE(linkMenu->isComplete());
}

TEST_F(LinkMenuTest, menuComplete_writesConfigToCardFails_promptsError)
{
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(false));
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();

    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_ERROR_CARDWRITE, true)};

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->confirm();
}

TEST_F(LinkMenuTest, menuComplete_writesConfigToCardFails_abortsMenu)
{
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(false));
    // enter, select folderId1, saveFolderId
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    EXPECT_FALSE(linkMenu->isActive());
    EXPECT_FALSE(linkMenu->isComplete());
}

TEST_F(LinkMenuTest, confirmFolderIdAndPlayMode_isActive_returnsTrue)
{
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(true));
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
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(true));
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
    EXPECT_CALL(m_messageHandlerMock, promptMessage(invalidPrompt()));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderId_noSelection_getPrompt_FolderIdAndAllowSkip)
{
    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_SELECT_FOLDERID, true)};

    linkMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayMode_noSelection_getPrompt_PlayModeAndAllowSkip)
{
    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_SELECT_PLAYMODE, true)};

    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, menuComplete_noSelection_getPrompt_TagConfigSuccess)
{
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(true));
    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_TAGCONFSUCCESS, true)};

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, menuComplete_noSelection_confirmAgainAndGetPrompt_noPromptSet)
{
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(true));
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(invalidPrompt()));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderId1_getPrompt_returns1)
{
    VoicePrompt expect{VoicePrompt(1, false)};

    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPrompt_returnsMAX)
{
    VoicePrompt expect{VoicePrompt(MAXFOLDERCOUNT, false)};

    linkMenu->confirm();
    linkMenu->selectPrev();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayMode1_getPrompt_returnsALBUM)
{
    VoicePrompt expect{VoicePrompt(static_cast<uint16_t>(VoicePrompt::MSG_SELECT_PLAYMODE) +
                                       static_cast<uint16_t>(Folder::ALBUM),
                                   false)};

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayModeMAX_getPrompt_returnsONELARGETRACK)
{
    VoicePrompt expect{VoicePrompt(static_cast<uint16_t>(VoicePrompt::MSG_SELECT_PLAYMODE) +
                                       static_cast<uint16_t>(Folder::ONELARGETRACK),
                                   false)};

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectPrev();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectPlayMode_testRollover_getPrompt_returnsSAVEPROGRESS)
{
    VoicePrompt expect{VoicePrompt(static_cast<uint16_t>(VoicePrompt::MSG_SELECT_PLAYMODE) +
                                       static_cast<uint16_t>(Folder::SAVEPROGRESS),
                                   false)};

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectPrev();
    linkMenu->selectPrev();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, noInit_abort_noPromptSet)
{
    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_ABORTED, true)};

    linkMenu->abort();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, folderId1Selected_abort_promptsAborted)
{
    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_ABORTED, true)};

    linkMenu->confirm();
    linkMenu->selectNext();

    linkMenu->abort();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, folderIdAndPlayModeSelected_abort_promptsAborted)
{
    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_ABORTED, true)};

    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();

    linkMenu->abort();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, menuCompleted_abort_promptsAborted)
{
    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_ABORTED, true)};

    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();

    linkMenu->abort();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    linkMenu->handlePlayback();
}

// Folder Preview Tests
TEST_F(LinkMenuTest, selectFolderId1_getPreview_returnsPreviewFolder1)
{
    Folder expect{Folder(1, Folder::ONELARGETRACK)};

    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_mp3PlayMock, playFolder(identicalFolder(expect)));
    linkMenu->handlePlayback();
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPreview_returnsPreviewFolderMAX)
{
    Folder expect{Folder(MAXFOLDERCOUNT, Folder::ONELARGETRACK)};

    linkMenu->confirm();
    linkMenu->selectPrev();

    EXPECT_CALL(m_mp3PlayMock, playFolder(identicalFolder(expect)));
    linkMenu->handlePlayback();
}