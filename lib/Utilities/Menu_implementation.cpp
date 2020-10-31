#include "Menu_implementation.h"

Menu_interface *Menu_factory::getInstance(eMenuType menuType)
{
    Menu_interface *instance;
    switch (menuType)
    {
    case DELETE_MENU:
        instance = new DeleteMenu();
        break;
    case LINK_MENU:
        instance = new LinkMenu();
        break;
    }
    return instance;
}
