#ifndef MENU_IMPLEMENTATION_H
#define MENU_IMPLEMENTATION_H

#include "Menu_interface.h"

#include "LinkMenu/LinkMenu.h"
#include "DeleteMenu/DeleteMenu.h"

class NfcControl_interface;
class Mp3Play_interface;

class Menu_factory
{
public:
    enum eMenuType
    {
        DELETE_MENU = 0,
        LINK_MENU
    };

public:
    static Menu_interface *getInstance(eMenuType menuType,
                                       NfcControl_interface *pNfcCtrl,
                                       Mp3Play_interface *pMp3Play,
                                       MessageHander_interface *pMessageHandler,
                                       PowerManager_interface *pPowerMgr);
};

#endif // MENU_IMPLEMENTATION_H