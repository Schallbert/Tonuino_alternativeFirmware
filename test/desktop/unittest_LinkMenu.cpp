#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Folder.h>
#include <LinkMenu.h>

// INIT() ------------------------------------------------------
TEST(linkMenu, noInit_StateIsNO_MENU)
{
    LinkMenu menu;
    ASSERT_TRUE(menu.get_state() == LinkMenu::NO_MENU);
}

TEST(linkMenu, noInit_FolderIdIs0)
{
    LinkMenu menu;
    ASSERT_EQ(menu.get_folderId(), 0);
}

TEST(linkMenu, noInit_PlayModeIsUndefined)
{
    LinkMenu menu;
    ASSERT_EQ(menu.get_playMode(), Folder::UNDEFINED);
}

TEST(linkMenu, init_StateTransitionsToFOLDER_SELECT)
{
    LinkMenu menu;
    menu.init();
    ASSERT_TRUE(menu.get_state() == LinkMenu::FOLDER_SELECT);
}

TEST(linkMenu, init_FolderIdIs0)
{
    LinkMenu menu;
    menu.init();
    ASSERT_EQ(menu.get_folderId(), 0);
}

TEST(linkMenu, init_PlayModeIsUndefined)
{
    LinkMenu menu;
    menu.init();
    ASSERT_EQ(menu.get_playMode(), Folder::UNDEFINED);
}

TEST(linkMenu, noInit_selectConfirm_StateIsNOMENU)
{
    LinkMenu menu;
    menu.select_confirm();
    ASSERT_TRUE(menu.get_state() == LinkMenu::NO_MENU);
}

TEST(linkMenu, noInit_selectNext_FolderIdStays0)
{
    LinkMenu menu;
    menu.select_next();
    ASSERT_EQ(menu.get_folderId(), 0);
}

TEST(linkMenu, noInit_selectPrev_FolderIdStays0)
{
    LinkMenu menu;
    menu.select_prev();
    ASSERT_EQ(menu.get_folderId(), 0);
}

// SELECT_CONFIRM() ------------------------------------------------------
TEST(linkMenu, selectConfirm_StateTransitionsToPLAYMODE_SELECT)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    ASSERT_TRUE(menu.get_state() == LinkMenu::PLAYMODE_SELECT);
}

TEST(linkMenu, selectConfirm_folderIdIs0)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    ASSERT_EQ(menu.get_folderId(), 0);
}

TEST(linkMenu, selectNext1_returnValueIs1)
{
    LinkMenu menu;
    menu.init();
    ASSERT_EQ(menu.select_next(), 1);
}

TEST(linkMenu, confirm2_MenuIsCOMPLETE)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    menu.select_confirm();
    ASSERT_TRUE(menu.get_state() == LinkMenu::COMPLETE);
}

TEST(linkMenu, confirm2_PlayModeIsUNDEFINED)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::UNDEFINED);
}

// SELECT_NEXT() ------------------------------------------------------
TEST(linkMenu, init_selectNext1_noConfirm_FolderIdStays0)
{
    LinkMenu menu;
    menu.init();
    menu.select_next();
    ASSERT_EQ(menu.get_folderId(), 0);
}

TEST(linkMenu, selectNext1_confirm_FolderIdIs1)
{
    LinkMenu menu;
    menu.init();
    menu.select_next();
    menu.select_confirm();
    ASSERT_EQ(menu.get_folderId(), 1);
}

TEST(linkMenu, selectNextMAX_confirm_FolderIdIsMAXFOLDERCOUNT)
{
    LinkMenu menu;
    menu.init();
    for (int i = 0; i < (MAXFOLDERCOUNT); ++i)
    {
        menu.select_next();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_folderId(), MAXFOLDERCOUNT);
}

TEST(linkMenu, selectNextRollOver_confirm_FolderIdIs1)
{
    LinkMenu menu;
    menu.init();
    for (int i = 0; i < (MAXFOLDERCOUNT + 1); ++i)
    {
        menu.select_next();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_folderId(), 1);
}

TEST(linkMenu, selectNext_confirm_PlayModeIsUndefined)
{
    LinkMenu menu;
    menu.init();
    menu.select_next();
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::UNDEFINED);
}

TEST(linkMenu, playModeSelect_selectNext_returnValueIsOFFSETplus1)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    ASSERT_EQ(menu.select_next(), MSG_SELECT_PLAYMODE+1);
}

TEST(linkMenu, playModeSelect_selectNext_confirm_getPlaymodeReturnsALBUM)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    menu.select_next();
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::LULLABYE);
}

TEST(linkMenu, playModeSelect_selectNext_confirm_getPlaymodeReturnsFirstValidEnum)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    menu.select_next();
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::LULLABYE);
}

TEST(linkMenu, playModeSelect_selectNextMAX_confirm_getPlaymodeReturnsLastValidEnum)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    for( int i = 0; i < Folder::ENUM_COUNT; ++i)
    {
        menu.select_next();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::ONELARGETRACK);
}

TEST(linkMenu, playModeSelect_selectNextRollover_confirm_getPlaymodeReturnsFirstValidEnum)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    for( int i = 0; i < (Folder::ENUM_COUNT + 1); ++i)
    {
        menu.select_next();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::LULLABYE);
}

TEST(linkMenu, selectPrev1_returnValueIsMAXFOLDERCNT)
{
    LinkMenu menu;
    menu.init();
    ASSERT_EQ(menu.select_prev(), MAXFOLDERCOUNT);
}

TEST(linkMenu, selectPrev1_NoConfirm_FolderIdStays0)
{
    LinkMenu menu;
    menu.init();
    menu.select_prev();
    ASSERT_EQ(menu.get_folderId(), 0);
}

TEST(linkMenu, selectPrev1_confirm_FolderIdIsMAXFOLDERCNT)
{
    LinkMenu menu;
    menu.init();
    menu.select_prev();
    menu.select_confirm();
    ASSERT_EQ(menu.get_folderId(), MAXFOLDERCOUNT);
}

TEST(linkMenu, selectPrevMAX_confirm_FolderIdIs1)
{
    LinkMenu menu;
    menu.init();
    for (int i = 0; i < (MAXFOLDERCOUNT); ++i)
    {
        menu.select_prev();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_folderId(), 1);
}

// SELECT_PREV() ------------------------------------------------------
TEST(linkMenu, selectPrevRollOver_confirm_FolderIdIsMAXFOLDERCNT)
{
    LinkMenu menu;
    menu.init();
    for (int i = 0; i < (MAXFOLDERCOUNT + 1); ++i)
    {
        menu.select_prev();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_folderId(), MAXFOLDERCOUNT);
}

TEST(linkMenu, playModeSelect_selectPrev_confirm_getPlaymodeReturnsLastValidEnum)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    menu.select_prev();
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::ONELARGETRACK);
}

TEST(linkMenu, playModeSelect_selectPrevMIN_confirm_getPlaymodeReturnsFirstValidEnum)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    for( int i = 0; i < Folder::ENUM_COUNT; ++i)
    {
        menu.select_prev();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::LULLABYE);
}

TEST(linkMenu, playModeSelect_selectPrevRollover_confirm_getPlaymodeReturnsLastValidEnum)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    for( int i = 0; i < (Folder::ENUM_COUNT + 1); ++i)
    {
        menu.select_prev();
    }
    menu.select_confirm();
    ASSERT_EQ(menu.get_playMode(), Folder::ONELARGETRACK);
}

// SELECT_ABORT() ------------------------------------------------------
TEST(linkMenu, selectAbort_menuStateIsReset)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    menu.select_abort();
    ASSERT_TRUE(menu.get_state() == LinkMenu::NO_MENU);
}

TEST(linkMenu, selectAbort_folderIdIsReset)
{
    LinkMenu menu;
    menu.init();
    menu.select_prev();
    menu.select_confirm();
    menu.select_abort();
    ASSERT_EQ(menu.get_folderId(), 0);
}

TEST(linkMenu, selectAbort_playModeIsReset)
{
    LinkMenu menu;
    menu.init();
    menu.select_confirm();
    menu.select_prev();
    menu.select_confirm();
    menu.select_abort();
    ASSERT_EQ(menu.get_playMode(), Folder::UNDEFINED);
}

TEST(linkMenu, selectAbort_selectPrevReturns0)
{
    LinkMenu menu;
    menu.init();
    menu.select_prev();
    menu.select_confirm();
    menu.select_abort();
    ASSERT_EQ(menu.select_prev(), 0);
}

TEST(linkMenu, selectAbort_reInit_selectPrevReturnsMAX)
{
    LinkMenu menu;
    menu.init();
    menu.select_prev();
    menu.select_confirm();
    menu.select_abort();
    menu.init();
    ASSERT_EQ(menu.select_prev(), MAXFOLDERCOUNT);
}

TEST(linkMenu, selectAbort_selectNextReturns0)
{
    LinkMenu menu;
    menu.init();
    menu.select_next();
    menu.select_confirm();
    menu.select_abort();
    ASSERT_EQ(menu.select_next(), 0);
}

TEST(linkMenu, selectAbort_reInit_selectNextReturns1)
{
    LinkMenu menu;
    menu.init();
    menu.select_next();
    menu.select_confirm();
    menu.select_abort();
    menu.init();
    ASSERT_EQ(menu.select_next(), 1);
}