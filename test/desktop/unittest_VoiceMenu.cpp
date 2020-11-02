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

TEST_F(VoiceMenuTest, initLinkMenu_playPrompt_playsSelectFolderIdPrompt)
{
    InputState input{}; 
    input.tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    m_pVoiceMenu->setInputState(input);

    EXPECT_CALL(*m_pPromptPlayer, checkPlayPrompt(_));
    EXPECT_CALL(*m_pPromptPlayer, checkPlayFolderPreview(_));

    m_pVoiceMenu->loop();

    
}



//  m_deleteMenu = Menu_factory::getInstance(Menu_factory::DELETE_MENU);