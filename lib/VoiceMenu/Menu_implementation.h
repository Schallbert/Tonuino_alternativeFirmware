#ifndef MENU_IMPLEMENTATION_H
#define MENU_IMPLEMENTATION_H

#include "Menu_interface.h"
#include "LinkMenu/LinkMenu.h"
#include "DeleteMenu/DeleteMenu.h"

class Menu_factory
{
public:
    enum eMenuType
    {
        DELETE_MENU = 0,
        LINK_MENU
    };

public:
    static Menu_interface *getInstance(eMenuType menuType);
};

#endif // MENU_IMPLEMENTATION_H