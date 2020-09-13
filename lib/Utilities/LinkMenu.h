#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "../Config/Tonuino_config.h"
#include <Folder.h>

/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
*/
class LinkMenu
{
public:
    enum eLinkMenuState
    {
        NO_MENU = 0,
        FOLDER_SELECT,
        PLAYMODE_SELECT,
        COMPLETE
    };

public:
    LinkMenu(){};

public:
    // Initializes menu and sets according variables
    void init();
    // getters for member variables
    bool is_state(eLinkMenuState state) { return (m_eMenuState == state); };
    uint8_t get_folderId() { return m_ui16FolderId; };
    Folder::ePlayMode get_playMode() { return m_ePlayMode; }
    // returns true if configuring is complete
    void select_confirm();
    // resets menu and resets variables
    void select_abort();
    // selects next menu item, e.g. next folderId and returns new selection
    uint16_t select_next();
    // selects previous menu item, e.g. previous folderId and returns new selection
    uint16_t select_prev();

    private: 
    // resets menu state if menu has not been correctly setup so that the optionRange field is filled
    bool check_initialized();

private:
    // constants for definition of offset
    // for playing prompts based on selected options
    const uint16_t START_OPTION_FOLDERID = 0;
    const uint16_t START_OPTION_PLAYMODE = MSG_SELECT_PLAYMODE;
    // initialized for folderId state of linkMenu   
    eLinkMenuState m_eMenuState{NO_MENU};
    uint16_t m_ui16FolderId{START_OPTION_FOLDERID};
    Folder::ePlayMode m_ePlayMode{Folder::UNDEFINED};
    uint16_t m_ui16Option{0};
    uint16_t m_ui16OptionOffset{0};
    uint16_t m_ui16OptionRange{0};


};

#endif // VOICEMENU_H