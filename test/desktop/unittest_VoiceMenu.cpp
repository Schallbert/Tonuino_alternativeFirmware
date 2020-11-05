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

#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer.h"

#include "mocks/unittest_PromptPlayer_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class VoiceMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pPromptPlayer = new NiceMock<Mock_PromptPlayer>;
        m_pMenuTimer = new SimpleTimer{};

        m_pVoiceMenu = new VoiceMenu(m_pPromptPlayer, m_pMenuTimer);
    }

    virtual void TearDown()
    {
        delete m_pVoiceMenu;

        delete m_pPromptPlayer;
        delete m_pMenuTimer;
    }

protected:
    Menu_interface *m_deleteMenu{nullptr};

protected:
    NiceMock<Mock_PromptPlayer> *m_pPromptPlayer{nullptr};
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
    InputState input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop();

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuComplete_isActive_returnsTrue)
{
     InputState input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop(); // enters Menu: select folderId
    input.btnState = UserInput::PLAY_PAUSE;
    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop(); // select playMode
    input.btnState = UserInput::PLAY_PAUSE;
    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop(); // should complete the menu

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuCompleteAndCalledAgain_isActive_returnsFalse)
{
     InputState input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop(); // enters Menu: select folderId
    input.btnState = UserInput::PLAY_PAUSE;
    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop(); // select playMode
    input.btnState = UserInput::PLAY_PAUSE;
    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop(); // should complete the menu
    m_pVoiceMenu->loop(); // should leave the menu

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, initLinkMenu_loop_invokesCheckPlayPromptAndFolderPreview)
{
    InputState input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;
    VoicePrompt selFolderId{};
    selFolderId.promptId = MSG_SELECT_FOLDERID;
    Folder emptyFolder{};


    m_pVoiceMenu->setInputState(input);

    //EXPECT_CALL(*m_pPromptPlayer, checkPlayPrompt(PromptIdsAreEqual(selFolderId)));
    //XPECT_CALL(*m_pPromptPlayer, playFolderPreview(FoldersAreEqual(emptyFolder)));

    EXPECT_CALL(*m_pPromptPlayer, checkPlayPrompt(_));
    EXPECT_CALL(*m_pPromptPlayer, playFolderPreview(_));

    m_pVoiceMenu->loop();
}

TEST_F(VoiceMenuTest, initDeleteMenu_loop_invokesCheckPlayPromptAndFolderPreview)
{
    InputState input{};
    input.tagState = Nfc_interface::ACTIVE_KNOWN_TAG;
    input.btnState = UserInput::PP_LONGPRESS;

    m_pVoiceMenu->setInputState(input);

    EXPECT_CALL(*m_pPromptPlayer, checkPlayPrompt(_));
    EXPECT_CALL(*m_pPromptPlayer, playFolderPreview(_));

    m_pVoiceMenu->loop();
}

TEST_F(VoiceMenuTest, linkMenuFolderId_loop_invokesCheckPlayPromptAndFolderPreview)
{
    InputState input{};
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop();

    input.btnState = UserInput::NEXT_TRACK;

    EXPECT_CALL(*m_pPromptPlayer, checkPlayPrompt(_));
    EXPECT_CALL(*m_pPromptPlayer, playFolderPreview(_));

    m_pVoiceMenu->setInputState(input);
    m_pVoiceMenu->loop();
}



// Test: menu instance is deleted