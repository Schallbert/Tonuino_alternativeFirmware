#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Utilities/Menu_interface.h"
#include "../Utilities/DeleteMenu.h"
#include "../Utilities/Menu_implementation.h"

class DeleteMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        deleteMenu = Menu_factory::getInstance(Menu_factory::DELETE_MENU);
    }

    virtual void TearDown()
    {
        delete deleteMenu;
    }

protected:
    Menu_interface *deleteMenu{nullptr};
};

// INIT() ------------------------------------------------------

TEST_F(DeleteMenuTest, noInit_noPromptSet)
{
    ASSERT_EQ((deleteMenu->getPrompt()).promptId, 0);
}

TEST_F(DeleteMenuTest, init_getPrompt_promptsDeleteTag)
{
    deleteMenu->confirm();

    Menu_interface::VoicePrompt result = deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_DELETETAG);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, init2x_getPrompt_promptsDeleteTag)
{
    deleteMenu->confirm();
    deleteMenu->confirm();

    Menu_interface::VoicePrompt result = deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_DELETETAG);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, placedTagToDelete_getPrompt_promptsWaitForConfirm)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    deleteMenu->confirm();
    deleteMenu->getLockedResponse(tagState);

    Menu_interface::VoicePrompt result = deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_CONFIRM_DELETION);
    ASSERT_EQ(result.allowSkip, true);

}

TEST_F(DeleteMenuTest, deleteMenuEntered_lockedInDeleteMenu)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_UNKNOWN_TAG;

    deleteMenu->confirm();
    deleteMenu->getLockedResponse(tagState);

    ASSERT_EQ(tagState, Nfc_interface::DELETE_TAG_MENU);
}
