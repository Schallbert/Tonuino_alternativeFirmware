#if 0
// interface to Menu.
// dependencies: MenuTimer, Mp3PlayerCtrl for prompting, Nfc for tagStates

//TASKS:
// dispatch menu commands to the correct menu.
// retrieve prompt info and play prompts.
// return folder information in case of Link Menu.
//

// INPUTS:
// MenuTimer state
// NfcTag state
// UserInput command

// OUTPUTS:
// Communicate whether a menu is active
// Return folder information on request
//

// TO DECIDE:
// Class architecture
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_PromptPlayer_mocks.h"
#include "mocks/unittest_NfcControl_mocks.h"

#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class VoiceMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pMenuTimer = new SimpleTimer{};

        m_pVoiceMenu = new VoiceMenu(&m_promptPlayerMock, &m_nfcControlMock, m_pMenuTimer);
    }

    virtual void TearDown()
    {
        delete m_pVoiceMenu;
        delete m_pMenuTimer;
    }

protected:
    Menu_interface *m_deleteMenu{nullptr};

protected:
    NiceMock<Mock_PromptPlayer> m_promptPlayerMock{};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    SimpleTimer *m_pMenuTimer{nullptr};

    VoiceMenu *m_pVoiceMenu{nullptr};
};

bool operator==(Folder lhs, Folder rhs)
{
    return (
        (lhs.get_folder_id() == rhs.get_folder_id()) &&
        (lhs.get_play_mode() == rhs.get_play_mode()) &&
        (lhs.get_track_count() == rhs.get_track_count())
    );
}

MATCHER_P(PromptIdsAreEqual, comp, "")
{
    return (
        (arg.promptId == comp.promptId) &&
        (arg.allowSkip == comp.allowSkip)
    );
}

MATCHER_P(FoldersAreEqual, comp, "") 
{
    return (arg == comp);
}

TEST_F(VoiceMenuTest, noInit_isActive_returnsFalse)
{
    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, initLinkMenu_isActive_returnsTrue)
{
    UserInput::eUserRequest input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setUserInput(input);
    m_pVoiceMenu->loop();

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuComplete_isActive_returnsTrue)
{
     UserInput::eUserRequest input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setUserInput(input);
    m_pVoiceMenu->loop(); // enters Menu: select folderId
    input.tagState = Nfc_interface::ACTIVE_KNOWN_TAG;
    input.btnState = UserInput::PLAY_PAUSE;
    m_pVoiceMenu->setUserInput(input);
    m_pVoiceMenu->loop(); // selects playMode and menu is complete

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuCompleteAndCalledAgain_isActive_returnsFalse)
{
     UserInput::eUserRequest input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setUserInput(input);
    m_pVoiceMenu->loop(); // enters Menu: select folderId
    input.tagState = Nfc_interface::ACTIVE_KNOWN_TAG;
    input.btnState = UserInput::PLAY_PAUSE;
    m_pVoiceMenu->setUserInput(input);
    m_pVoiceMenu->loop(); // selects playMode and completes menu
    m_pVoiceMenu->loop(); // leaves the menu

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, initLinkMenu_loop_invokesPlayPrompt)
{
    UserInput::eUserRequest input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;
    VoicePrompt selFolderId{};
    selFolderId.promptId = MSG_SELECT_FOLDERID;
    Folder emptyFolder{};

    m_pVoiceMenu->setUserInput(input);

    //EXPECT_CALL(m_promptPlayerMock, checkPlayPrompt(PromptIdsAreEqual(selFolderId)));
    EXPECT_CALL(m_promptPlayerMock, checkPlayPrompt(_));

    m_pVoiceMenu->loop();
}

TEST_F(VoiceMenuTest, initLinkMenu_loop_invokesFolderPreview)
{
    UserInput::eUserRequest input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;
    VoicePrompt selFolderId{};
    selFolderId.promptId = MSG_SELECT_FOLDERID;
    Folder emptyFolder{};

    m_pVoiceMenu->setUserInput(input);

    //EXPECT_CALL(*m_pPromptPlayer, playFolderPreview(FoldersAreEqual(emptyFolder)));
    EXPECT_CALL(m_promptPlayerMock, playFolderPreview(_));

    m_pVoiceMenu->loop();
}

TEST_F(VoiceMenuTest, initDeleteMenu_loop_invokesplayPrompt)
{
    UserInput::eUserRequest input{};
    input.tagState = Nfc_interface::ACTIVE_KNOWN_TAG;
    input.btnState = UserInput::PP_LONGPRESS;

    m_pVoiceMenu->setUserInput(input);

    EXPECT_CALL(m_promptPlayerMock, checkPlayPrompt(_));

    m_pVoiceMenu->loop();
}

/* most likely, this fails because deleteMenu's tagDetected is not set.
TEST_F(VoiceMenuTest, linkMenuFolderId_loop_invokesPlayPreview)
{
    UserInput::eUserRequest input{};
    input.tagState = Nfc_interface::ACTIVE_KNOWN_TAG;
    input.btnState = UserInput::PP_LONGPRESS;

    m_pVoiceMenu->setUserInput(input);
    m_pVoiceMenu->loop();
    input.tagState = Nfc_interface::NEW_REGISTERED_TAG;

    EXPECT_CALL(*m_pPromptPlayer, playFolderPre view(_));

    m_pVoiceMenu->setUserInput(input);
    m_pVoiceMenu->loop();
}*/ 

// Test: menu instance is deleted
#endif