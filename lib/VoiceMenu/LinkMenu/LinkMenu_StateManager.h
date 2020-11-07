#ifndef LINKMENU_STATEMANAGER_h
#define LINKMENU_STATEMANAGER_h

#include "../Arduino/Arduino_interface/Arduino_types.h"
#include "../Config/Tonuino_config.h"

#include "../Folder/Folder.h"

class LinkMenu_StateManager
{
public:
    void confirm();
    void abort();
    void incrementSelection();
    void decrementSelection();

    uint16_t getMenuStateMessage();
    uint16_t getCurrentSelection();
    Folder getResult();
    Folder getPreview();

private:
    void initFolderIdSelection();
    void saveFolderIdSelection();
    void initPlayModeSelection();
    void savePlayModeSelection();

private:
    static const uint16_t START_OPTION_FOLDERID = 0;
    static const uint16_t START_OPTION_PLAYMODE = MSG_SELECT_PLAYMODE;

    enum eMenuState
    {
        NO_MENU = 0,
        FOLDERID_SELECT = MSG_SELECT_FOLDERID,
        PLAYMODE_SELECT = MSG_SELECT_PLAYMODE,
        COMPLETE = MSG_TAGCONFSUCCESS
    };

    class MenuOptions
    {
    public:
        void resetForFolderId();
        void resetForPlayMode();
        void incrementSelection();
        void decrementSelection();

        uint16_t getSelection() { return selection; };    

    private:
        uint16_t selection{START_OPTION_FOLDERID};
        uint16_t offset{0};
        uint16_t range{MAXFOLDERCOUNT};
    };

    struct MenuSelection
    {
        void reset();

        uint8_t folderId{0};
        Folder::ePlayMode playMode{Folder::UNDEFINED};
    };

    eMenuState menuState{NO_MENU};

    MenuOptions options{};
    MenuSelection result{};
};

#endif // LINKMENU_STATEMANAGER_h
