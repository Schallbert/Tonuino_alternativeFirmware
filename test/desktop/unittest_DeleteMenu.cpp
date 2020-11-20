#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"

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
                                               &m_Mp3PlayMock,
                                               &m_powerManagerMock);
    }

    virtual void TearDown()
    {
        delete deleteMenu;
    }

protected:
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_Mp3Play> m_Mp3PlayMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
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
    return (!arg.is_initiated());
}

MATCHER(validFolder, "")
{
    return (arg.is_initiated());
}

MATCHER_P(identicalFolder, comp, "")
{
    return ((arg.get_folder_id() == comp.get_folder_id()) &&
            (arg.get_play_mode() == comp.get_play_mode()) &&
            (arg.get_track_count() == comp.get_track_count()));
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

TEST_F(DeleteMenuTest, enteredAbort_isActive_eturnsFalse)
{
    deleteMenu->confirm();
    deleteMenu->abort();

    ASSERT_FALSE((deleteMenu->isActive()));
}

TEST_F(DeleteMenuTest, menuComplete_isActive_returnsTrue)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    ASSERT_TRUE((deleteMenu->isActive()));
}

TEST_F(DeleteMenuTest, noInit_setStatusLed_noStatusLedChangeRequested)
{
    EXPECT_CALL(m_powerManagerMock, set_delMenu()).Times(0);
    deleteMenu->setStatusLed();
}

TEST_F(DeleteMenuTest, entered_setStatusLed_statusLedSetToDeleteMenu)
{
    deleteMenu->confirm();

    EXPECT_CALL(m_powerManagerMock, set_delMenu());
    deleteMenu->setStatusLed();
}

TEST_F(DeleteMenuTest, menuComplete_setStatusLed_statusLedChangeRequested)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    EXPECT_CALL(m_powerManagerMock, set_delMenu());
    deleteMenu->setStatusLed();
}

TEST_F(DeleteMenuTest, menuAbort_setStatusLed_noStatusLedChangeRequested)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->abort();      

    EXPECT_CALL(m_powerManagerMock, set_delMenu()).Times(0);
    deleteMenu->setStatusLed();
}

// PROMPT tests
TEST_F(DeleteMenuTest, noInit_noPromptSet)
{
    EXPECT_CALL(m_Mp3PlayMock, playPrompt(invalidPrompt()));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, init_getPrompt_promptsDeleteTag)
{
    VoicePrompt expect{};
    expect.promptId = MSG_DELETETAG;
    expect.allowSkip = true;

    deleteMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, init2x_getPrompt_promptsDeleteTag)
{
    VoicePrompt expect{};
    expect.promptId = MSG_DELETETAG;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, placedTagToDelete_getPrompt_promptsWaitForConfirm)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    VoicePrompt expect{};
    expect.promptId = MSG_CONFIRM_DELETION;
    expect.allowSkip = true;

    deleteMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, placedTagToDelete_confirmDeletion_promptsTagConfigurationComplete)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    VoicePrompt expect{};
    expect.promptId = MSG_TAGCONFSUCCESS;
    expect.allowSkip = true;

    deleteMenu->confirm();        // enter
    deleteMenu->handlePlayback(); // detects tag to delete
    deleteMenu->confirm();        //confirms deletion

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, noInit_abort_noPromptSet)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    deleteMenu->abort();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, entered_abort_noPromptSet)
{
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->abort();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, tagToDeleteDetected_abort_noPromptSet)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    VoicePrompt expect{};
    expect.promptId = MSG_ABORTED;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->handlePlayback();
    deleteMenu->abort();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, menuComplete_abort_reentry_promptsDeleteTag)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));

    VoicePrompt expect{};
    expect.promptId = MSG_CONFIRM_DELETION;
    expect.allowSkip = true;

    deleteMenu->confirm();
    deleteMenu->handlePlayback();
    deleteMenu->abort();

    deleteMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(identicalPrompt(expect)));
    deleteMenu->handlePlayback();
}

// PREVIEW tests
TEST_F(DeleteMenuTest, cardToDeleteDetected_cardPresent_playsPreview)
{
    ON_CALL(m_nfcControlMock, read_folder_from_card(_)).WillByDefault(Return(true));
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    Folder expect; // did not delegate to fake so read method will not do anything

    deleteMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playFolder(identicalFolder(expect)));
    deleteMenu->handlePlayback();
}

TEST_F(DeleteMenuTest, cardToDeleteDetected_noCard_noPreviewPlayed)
{
    ON_CALL(m_nfcControlMock, read_folder_from_card(_)).WillByDefault(Return(false));
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));

    deleteMenu->confirm();

    EXPECT_CALL(m_Mp3PlayMock, playFolder(_)).Times(0);
    deleteMenu->handlePlayback();
}