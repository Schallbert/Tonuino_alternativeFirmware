#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_Mp3Prompt_mocks.h"
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
                                             m_mp3PromptMock,
                                             m_messageHandlerMock,
                                             m_powerManagerMock);
    }

protected:
    Menu_interface *linkMenu{nullptr};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_Mp3Prompt> m_mp3PromptMock{};
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

    VoicePrompt expect{VoicePrompt(VoicePrompt::MSG_ERROR_CARDWRITE, VoicePrompt::PROMPT_NOSKIP)};
    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
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

// Prompt tests
TEST_F(LinkMenuTest, selectFolderId_noSelection_getPrompt_FolderIdAndAllowSkip)
{
    VoicePrompt expect{VoicePrompt::MSG_SELECT_FOLDERID, VoicePrompt::PROMPT_ALLOWSKIP};

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->confirm();
}

TEST_F(LinkMenuTest, selectPlayMode_noSelection_getPrompt_PlayModeAndAllowSkip)
{
    VoicePrompt expect{VoicePrompt::MSG_SELECT_PLAYMODE, VoicePrompt::PROMPT_ALLOWSKIP};
    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->confirm();
}

TEST_F(LinkMenuTest, menuComplete_noSelection_getPrompt_TagConfigSuccess)
{
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(true));
    VoicePrompt expect{VoicePrompt::MSG_TAGCONFSUCCESS, VoicePrompt::PROMPT_ALLOWSKIP};

    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->confirm();
}

TEST_F(LinkMenuTest, menuComplete_noSelection_confirmAgainAndGetPrompt_noPromptSet)
{
    ON_CALL(m_nfcControlMock, writeFolderToTag(_)).WillByDefault(Return(true));
    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(invalidPrompt()));
    linkMenu->confirm();
}

TEST_F(LinkMenuTest, selectFolderId1_getPrompt_returns1)
{
    VoicePrompt expect{1, VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->selectNext();
}

TEST_F(LinkMenuTest, selectFolderId10_getPrompt_returns10)
{
    VoicePrompt expect{10, VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->selectNN();
}

TEST_F(LinkMenuTest, selectFolderMAXminus10_getPrompt_returnsMAXminus10)
{
    VoicePrompt expect{MAXFOLDERCOUNT - 9, VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->selectPP();
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPrompt_returnsMAX)
{
    VoicePrompt expect{MAXFOLDERCOUNT, VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->selectPrev();
}

TEST_F(LinkMenuTest, selectPlayMode1_getPrompt_returnsALBUM)
{
    VoicePrompt expect{static_cast<uint16_t>(VoicePrompt::MSG_SELECT_PLAYMODE) +
                                       static_cast<uint16_t>(Folder::ALBUM),
                                   VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->selectNext();
}

TEST_F(LinkMenuTest, selectPlayModeMAX_getPrompt_returnsONELARGETRACK)
{
    VoicePrompt expect{static_cast<uint16_t>(VoicePrompt::MSG_SELECT_PLAYMODE) +
                                       static_cast<uint16_t>(Folder::ONELARGETRACK),
                                   VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();
    linkMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->selectPrev();
}

TEST_F(LinkMenuTest, selectPlayMode_testRollover_getPrompt_returnsSAVEPROGRESS)
{
    VoicePrompt expect{static_cast<uint16_t>(VoicePrompt::MSG_SELECT_PLAYMODE) +
                                       static_cast<uint16_t>(Folder::SAVEPROGRESS),
                                   VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();
    linkMenu->confirm();
    linkMenu->selectPrev();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->selectPrev();
}

TEST_F(LinkMenuTest, noInit_abort_noPromptSet)
{
    VoicePrompt expect{VoicePrompt::MSG_ABORTED, VoicePrompt::PROMPT_NOSKIP};
    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->abort();
}

TEST_F(LinkMenuTest, folderId1Selected_abort_promptsAborted)
{
    VoicePrompt expect{VoicePrompt::MSG_ABORTED, VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->abort();
}

TEST_F(LinkMenuTest, folderIdAndPlayModeSelected_abort_promptsAborted)
{
    VoicePrompt expect{VoicePrompt::MSG_ABORTED, VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->abort();
}

TEST_F(LinkMenuTest, menuCompleted_abort_promptsAborted)
{
    VoicePrompt expect{VoicePrompt::MSG_ABORTED, VoicePrompt::PROMPT_NOSKIP};

    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();
    linkMenu->selectNext();
    linkMenu->confirm();



    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    linkMenu->abort();
}

// Folder Preview Tests
TEST_F(LinkMenuTest, selectFolderId1_getPreview_returnsPreviewFolder1)
{
    Folder expect{1, Folder::ONELARGETRACK};

    linkMenu->confirm();

    EXPECT_CALL(m_mp3PlayMock, playFolder(identicalFolder(expect)));
    linkMenu->selectNext();
}

TEST_F(LinkMenuTest, selectFolderIdMAX_getPreview_returnsPreviewFolderMAX)
{
    Folder expect{MAXFOLDERCOUNT, Folder::ONELARGETRACK};

    linkMenu->confirm();

    EXPECT_CALL(m_mp3PlayMock, playFolder(identicalFolder(expect)));
    linkMenu->selectPrev();
}