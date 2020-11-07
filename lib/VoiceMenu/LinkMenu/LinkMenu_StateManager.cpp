#include "LinkMenu_StateManager.h"

void LinkMenu_StateManager::MenuOptions::resetForFolderId()
{
    selection = START_OPTION_FOLDERID;
    offset = 0;
    range = MAXFOLDERCOUNT;
}

void LinkMenu_StateManager::MenuOptions::resetForPlayMode()
{
    selection = LinkMenu_StateManager::START_OPTION_PLAYMODE;
    offset = LinkMenu_StateManager::START_OPTION_PLAYMODE;
    range = offset + static_cast<uint16_t>(Folder::ePlayMode::ENUM_COUNT);
}

void LinkMenu_StateManager::MenuOptions::incrementSelection()
{
    ++selection;
    if (selection <= offset ||
        selection > range)
    {
        selection = offset + 1;
    }
}

void LinkMenu_StateManager::MenuOptions::decrementSelection()
{
    --selection;
    if (selection <= offset ||
        selection > range)
    {
        selection = range;
    }
}


void LinkMenu_StateManager::MenuSelection::reset()
{
    folderId = 0;
    playMode = Folder::UNDEFINED;
}


void LinkMenu_StateManager::confirm()
{
    switch (menuState)
    {
    case NO_MENU:
        initFolderIdSelection();
        break;
    case FOLDERID_SELECT:
        saveFolderIdSelection();
        initPlayModeSelection();
        break;
    case PLAYMODE_SELECT:
        savePlayModeSelection();
        break;
    default: // menu complete or invalid key
        abort();
        break;
    }
}

void LinkMenu_StateManager::abort()
{
    result.reset();
    menuState = NO_MENU;
}

void LinkMenu_StateManager::initFolderIdSelection()
{
    options.resetForFolderId();
    result.reset();

    menuState = FOLDERID_SELECT;
}

void LinkMenu_StateManager::saveFolderIdSelection()
{
    result.folderId = static_cast<uint8_t>(options.getSelection() & 0xFF);
}

void LinkMenu_StateManager::initPlayModeSelection()
{
    result.playMode = Folder::UNDEFINED;

    options.resetForPlayMode();

    menuState = PLAYMODE_SELECT;
}

void LinkMenu_StateManager::savePlayModeSelection()
{
    result.playMode = static_cast<Folder::ePlayMode>(options.getSelection() - START_OPTION_PLAYMODE);

    menuState = COMPLETE;
}

void LinkMenu_StateManager::incrementSelection()
{
    options.incrementSelection();
}

void LinkMenu_StateManager::decrementSelection()
{
    options.decrementSelection();
}

uint16_t LinkMenu_StateManager::getCurrentSelection()
{
    return options.getSelection();
}

uint16_t LinkMenu_StateManager::getMenuStateMessage()
{
    return static_cast<uint16_t>(menuState);
}

Folder LinkMenu_StateManager::getResult()
{
    return Folder(result.folderId, result.playMode, 1);
}

Folder LinkMenu_StateManager::getPreview()
{
    // returns invalid folder if no preview is available
    Folder preview;
     if(menuState == FOLDERID_SELECT)
    {
        uint8_t folderId = static_cast<uint8_t>(options.getSelection() & 0xFF);
        Folder::ePlayMode playMode = Folder::ONELARGETRACK;
        preview = Folder(folderId, playMode, 1);
    }   
    return preview;
}
