#include "Menu_implementation.h"

Menu_interface *Menu_factory::getInstance(eMenuType menuType, NfcControl_interface *nfcCtrl)
{
    Menu_interface *instance;
    switch (menuType)
    {
    case DELETE_MENU:
        instance = new DeleteMenu(nfcCtrl);
        break;
    case LINK_MENU:
        instance = new LinkMenu(nfcCtrl);
        break;
    }
    return instance;
}
