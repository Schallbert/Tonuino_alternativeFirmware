#ifndef DELETEMENU_H
#define DELETEMENU_H

/* POD container for delete menu status. */
class DeleteMenu
{
public:
    enum eDelMenuState
    {
        NO_MENU = 0,
        DELETE_MENU,
        DELETE_READY
    };

public:
// initializes and controls delete process and plays voice prompt
    void init() { m_eMenuState = DELETE_MENU; }
    void set_state(eDelMenuState state) { m_eMenuState = state; }
    bool is_state(eDelMenuState state) { return (m_eMenuState == state); };

private:
    eDelMenuState m_eMenuState{NO_MENU};
};

#endif // DELETEMENU_H 