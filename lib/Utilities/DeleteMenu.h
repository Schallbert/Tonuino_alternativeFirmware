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
    void set_ready() { m_eMenuState = DELETE_READY; }
    void leave() {m_eMenuState = NO_MENU; }
    bool is_state(eDelMenuState state) { return (m_eMenuState == state); };

private:
    eDelMenuState m_eMenuState{NO_MENU};
};

#endif // DELETEMENU_H 