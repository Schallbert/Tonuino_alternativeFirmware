#ifndef VOICEMENU_H
#define VOICEMENU_H

//#include <Defines.h>
#include <Folder.h>
#include <EEPROM_interface.h>
#include <Mp3PlayerControl.h>

/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
*/
class LinkMenu
{
public:
    LinkMenu(Mp3PlayerControl *pMp3, EEPROM_interface *pEeprom) : m_pMp3(pMp3), m_pEeprom(pEeprom){};

public:
    // initializes linking process and plays voice prompt if TRUE, else reset object.
    void set_state(bool state);
    // returns true if link menu has been initialized
    bool get_state() { return m_bMenuState; };
    // returns true if configuring is complete
    bool select_confirm();
    // selects next menu item, e.g. next folderId
    void select_next();
    // selects previous menu item, e.g. previous folderId
    void select_prev();
    // returns a valid folder object when configuring is complete.
    Folder get_folder();

private:
    // plays the voice prompt selected option, adjusted to current menu state
    void play_voice_prompt();
    // Initializes values and prompts for folderId.
    void init();
    // resets internal state and values.
    void leave();

private:
    // needed for menu to be able to play voice prompts & previews
    Mp3PlayerControl *m_pMp3{nullptr};
    EEPROM_interface *m_pEeprom{nullptr};
    Folder m_linkedFolder{};
    // initialized for folderId state of linkMenu
    bool m_bMenuState{false};
    bool m_bLinkState{false};
    uint8_t m_ui8FolderId{0};
    uint8_t m_ui8Option{0};
    uint8_t m_ui8OptionRange{0};
};

#endif // VOICEMENU_H