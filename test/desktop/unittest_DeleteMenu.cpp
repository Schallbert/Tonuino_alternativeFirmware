#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "Menu_factory.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class DeleteMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        deleteMenu = Menu_factory::getInstance(Menu_factory::DELETE_MENU,
                                               &m_nfcControlMock,
                                               &m_mp3PlayMock,
                                               &m_messageHandlerMock,
                                               &m_powerManagerMock);
    }

    virtual void TearDown()
    {
        delete deleteMenu;
    }

protected:
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    Menu_interface *deleteMenu{nullptr};
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
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    ASSERT_TRUE((deleteMenu->isActive()));
}

TEST_F(DeleteMenuTest, noInit_setStatusLed_noStatusLedChangeRequested)
{
    EXPECT_CALL(m_powerManagerMock, setDeleteMenu()).Times(0);
    deleteMenu->setStatusLed();
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
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    EXPECT_CALL(m_powerManagerMock, setDeleteMenu());
    deleteMenu->setStatusLed();
}

TEST_F(DeleteMenuTest, menuComplete_callsEraseCard)
{
    EXPECT_CALL(m_nfcControlMock, eraseTag());

    deleteMenu->confirm();        // enter
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion
}

TEST_F(DeleteMenuTest, menuAbort_setStatusLed_noStatusLedChangeRequested)
{
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->abort();      

    EXPECT_CALL(m_powerManagerMock, setDeleteMenu()).Times(0);
    deleteMenu->setStatusLed();
}

// PROMPT tests
TEST_F(DeleteMenuTest, noInit_noPromptSet)
{
    EXPECT_CALL(m_messageHandlerMock, promptMessage(invalidPrompt()));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, init_getPrompt_promptsDeleteTag)
{
    VoicePrompt expect{};
    expect.promptId = MSG_DELETETAG;
    expect.allowSkip = true;

    deleteMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, init2x_getPrompt_promptsDeleteTag)
{
    VoicePrompt expect{};
    expect.promptId = MSG_DELETETAG;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, placedTagToDelete_getPrompt_promptsWaitForConfirm)
{
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    VoicePrompt expect{};
    expect.promptId = MSG_CONFIRM_DELETION;
    expect.allowSkip = true;

    deleteMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, placedTagToDelete_confirmDeletion_promptsTagConfigurationComplete)
{
    ON_CALL(m_nfcControlMock, eraseTag()).WillByDefault(Return(true));
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    VoicePrompt expect{};
    expect.promptId = MSG_TAGCONFSUCCESS;
    expect.allowSkip = true;

    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, noInit_abort_noPromptSet)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    deleteMenu->abort();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, entered_abort_noPromptSet)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->abort();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, tagToDeleteDetected_abort_noPromptSet)
{
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->handlePlayback();
    deleteMenu->abort();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, menuComplete_abort_reentry_promptsDeleteTag)
{
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);

    VoicePrompt expect{};
    expect.promptId = MSG_CONFIRM_DELETION;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->handlePlayback();
    deleteMenu->abort();

    deleteMenu->confirm();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

// PREVIEW tests
TEST_F(DeleteMenuTest, cardToDeleteDetected_cardPresent_playsPreview)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);
    Folder expect; // did not delegate to fake so read method will not do anything

    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PlayMock, playFolder(identicalFolder(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, cardToDeleteDetected_noCard_noPreviewPlayed)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(false));
    deleteMenu->setTagState(Nfc_interface::NEW_REGISTERED_TAG);

    deleteMenu->confirm();

    EXPECT_CALL(m_mp3PlayMock, playFolder(_)).Times(0);
    deleteMenu->handlePlayback();
}