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
#include "../Mp3/Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"

#include "mocks/unittest_Mp3PlayerControl_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class VoiceMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
       m_pMp3Ctrl = new NiceMock<Mock_Mp3PlayerControl>;
       m_pMenuTimer = new SimpleTimer{};

       m_pVoiceMenu = new VoiceMenu(m_pMp3Ctrl, m_pMenuTimer);
    }

    virtual void TearDown()
    {
        delete m_deleteMenu;
    }

protected:
    Menu_interface *m_deleteMenu{nullptr};

protected:
    NiceMock<Mock_Mp3PlayerControl> *m_pMp3Ctrl{nullptr};
    SimpleTimer *m_pMenuTimer{nullptr};

    VoiceMenu *m_pVoiceMenu{};
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

//  m_deleteMenu = Menu_factory::getInstance(Menu_factory::DELETE_MENU);