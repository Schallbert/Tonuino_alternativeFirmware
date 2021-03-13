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

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class DeleteMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        deleteMenu = m_MenuFactory.getInstance(Menu_factory::DELETE_MENU,
                                               m_nfcControlMock,
                                               m_mp3PlayMock,
                                               m_mp3PromptMock,
                                               m_messageHandlerMock,
                                               m_powerManagerMock);
    }

protected:
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_Mp3Prompt> m_mp3PromptMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    Menu_factory m_MenuFactory;
    Menu_interface *deleteMenu{nullptr};
};

// INIT() ------------------------------------------------------

TEST_F(DeleteMenuTest, noInit_isActive_returnsFalse)
{
    ASSERT_FALSE((deleteMenu->isActive()));
}

TEST_F(DeleteMenuTest, entered_isActive_returnsTrue)
{
    deleteMenu->confirm();

    ASSERT_TRUE((deleteMenu->isActive()));
}

TEST_F(DeleteMenuTest, enteredAbort_isActive_returnsFalse)
{
    deleteMenu->confirm();
    deleteMenu->abort();

    ASSERT_FALSE((deleteMenu->isActive()));
}

TEST_F(DeleteMenuTest, menuComplete_isActive_returnsTrue)
{
    ON_CALL(m_nfcControlMock, eraseTag()).WillByDefault(Return(true));
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    ASSERT_TRUE((deleteMenu->isActive()));
}

TEST_F(DeleteMenuTest, entered_setStatusLed_statusLedSetToDeleteMenu)
{
    deleteMenu->confirm();

    EXPECT_CALL(m_powerManagerMock, setDeleteMenu());
    deleteMenu->setStatusLed();
}

TEST_F(DeleteMenuTest, menuComplete_setStatusLed_statusLedChangeRequested)
{
    ON_CALL(m_nfcControlMock, eraseTag()).WillByDefault(Return(true));
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    EXPECT_CALL(m_powerManagerMock, setDeleteMenu());
    deleteMenu->setStatusLed();
}

TEST_F(DeleteMenuTest, menuComplete_callsEraseCard)
{
    EXPECT_CALL(m_nfcControlMock, eraseTag());

    deleteMenu->confirm(); // enter
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion
}

// PROMPT tests
TEST_F(DeleteMenuTest, noInit_noPromptSet)
{
    EXPECT_CALL(m_mp3PromptMock, playPrompt(invalidPrompt()));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, init_getPrompt_promptsDeleteTag)
{
    VoicePrompt expect{VoicePrompt::MSG_DELETETAG, VoicePrompt::ALLOWSKIP};
    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, init2x_getPrompt_promptsDeleteTag)
{
    VoicePrompt expect{VoicePrompt::MSG_DELETETAG, VoicePrompt::ALLOWSKIP};

    deleteMenu->confirm();
    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, placedTagToDelete_getPrompt_promptsWaitForConfirm)
{
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    VoicePrompt expect{VoicePrompt::MSG_CONFIRM_DELETION, VoicePrompt::ALLOWSKIP};

    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, placedTagToDelete_confirmDeletion_promptsTagConfigurationComplete)
{
    ON_CALL(m_nfcControlMock, eraseTag()).WillByDefault(Return(true));
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    VoicePrompt expect{VoicePrompt::MSG_TAGCONFSUCCESS, VoicePrompt::ALLOWSKIP};

    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, noInit_abort_noPromptSet)
{
    VoicePrompt expect{VoicePrompt::MSG_ABORTED, VoicePrompt::ALLOWSKIP};

    deleteMenu->abort();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, entered_abort_noPromptSet)
{
    VoicePrompt expect{VoicePrompt::MSG_ABORTED, VoicePrompt::ALLOWSKIP};

    deleteMenu->confirm();
    deleteMenu->abort();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, tagToDeleteDetected_abort_noPromptSet)
{
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    VoicePrompt expect{VoicePrompt::MSG_ABORTED, VoicePrompt::ALLOWSKIP};

    deleteMenu->confirm();
    deleteMenu->handlePlayback();
    deleteMenu->abort();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, menuComplete_abort_reentry_promptsDeleteTag)
{
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    VoicePrompt expect{VoicePrompt::MSG_CONFIRM_DELETION, VoicePrompt::ALLOWSKIP};

    deleteMenu->confirm();
    deleteMenu->handlePlayback();
    deleteMenu->abort();

    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PromptMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

// PREVIEW tests
TEST_F(DeleteMenuTest, cardToDeleteDetected_cardPresent_playsPreview)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
    deleteMenu->setTagState(Message::NEWKNOWNTAG);
    Folder expect; // did not delegate to fake so read method will not do anything

    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PlayMock, playFolder(identicalFolder(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, cardToDeleteDetected_noCard_noPreviewPlayed)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(false));
    deleteMenu->setTagState(Message::NEWKNOWNTAG);

    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PlayMock, playFolder(_)).Times(0);
    deleteMenu->handlePlayback();
}