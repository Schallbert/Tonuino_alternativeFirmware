#ifndef MENU_IMPLEMENTATION_H
#define MENU_IMPLEMENTATION_H

#include "Menu_interface.h"
#include "MessageHandler_interface.h"

class NfcControl_interface;
class Mp3Play_interface;
class Mp3Prompt_interface;
class PowerManager_interface;

class Menu_factory
{
public:
    enum eMenuType
    {
        DELETE_MENU = 0,
        LINK_MENU
    };

public:
    Menu_factory() = default;
    ~Menu_factory();
    Menu_factory(const Menu_factory &cpy) = delete;

    Menu_interface *getInstance(eMenuType menuType,
                                NfcControl_interface &rNfcCtrl,
                                Mp3Play_interface &rMp3Play,
                                Mp3Prompt_interface &rMp3Prompt,
                                MessageHander_interface &rMessageHandler,
                                PowerManager_interface &rPowerMgr);

private:
    void removeInstance();

private:
    Menu_interface *m_instance{nullptr};
};

#endif // MENU_IMPLEMENTATION_H