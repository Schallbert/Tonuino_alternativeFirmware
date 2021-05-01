#include <avr/pgmspace.h>

#include "MessageHandler_interface.h"
#include "messages.h"

// WARNING:
// Custom Message s_gr + ": " + s_ctt may not count
// more letters than DEBUG_MAXMESSAGELENGTH -1 or there will be SEGFAULTs!
const char s_error[] PROGMEM = "MESSAGE ERROR!";

#if DEBUGSERIAL
const char s_gr0[] PROGMEM = "System";
const char s_gr1[] PROGMEM = "Mp3Control";
const char s_gr2[] PROGMEM = "Mp3Playback";
const char s_gr3[] PROGMEM = "Mp3Player";
const char s_gr4[] PROGMEM = "NfcControl";
const char s_gr5[] PROGMEM = "NfcReader";
const char s_gr6[] PROGMEM = "Playlist";
const char s_gr7[] PROGMEM = "Input";

const char s_ctt00[] PROGMEM = "Started";
const char s_ctt01[] PROGMEM = "Shutdown";
const char s_ctt02[] PROGMEM = "Prompt timeout";
const char s_ctt03[] PROGMEM = "Init Error";

const char s_ctt10[] PROGMEM = "Volume up";
const char s_ctt11[] PROGMEM = "Volume down";
const char s_ctt12[] PROGMEM = "Next track";
const char s_ctt13[] PROGMEM = "Previous track";
const char s_ctt14[] PROGMEM = "Play";
const char s_ctt15[] PROGMEM = "Pause";

const char s_ctt20[] PROGMEM = "folder invalid!";
const char s_ctt21[] PROGMEM = "Folder: Play";
const char s_ctt22[] PROGMEM = "Autoplay complete";
const char s_ctt23[] PROGMEM = "Autoplay next";

const char s_ctt30[] PROGMEM = "Finished track";
const char s_ctt31[] PROGMEM = "SD card online";
const char s_ctt32[] PROGMEM = "SD card inserted";
const char s_ctt33[] PROGMEM = "SD Card removed";
const char s_ctt34[] PROGMEM = "Com Error";
const char s_ctt35[] PROGMEM = "Init";

const char s_ctt40[] PROGMEM = "No Tag";
const char s_ctt41[] PROGMEM = "Active Tag";
const char s_ctt42[] PROGMEM = "Same Tag";
const char s_ctt43[] PROGMEM = "New Tag";
const char s_ctt44[] PROGMEM = "Unknown Tag";
const char s_ctt45[] PROGMEM = "Removed Tag";

const char s_ctt51[] PROGMEM = "Tag Write Success";
const char s_ctt52[] PROGMEM = "Tag Read Success";
const char s_ctt53[] PROGMEM = "Tag write Error";
const char s_ctt54[] PROGMEM = "Tag read Error";
const char s_ctt55[] PROGMEM = "Tag type Error";
const char s_ctt56[] PROGMEM = "Tag online";

const char s_ctt60[] PROGMEM = "Mode Lullabye";
const char s_ctt61[] PROGMEM = "Mode Album";
const char s_ctt62[] PROGMEM = "Mode Random";
const char s_ctt63[] PROGMEM = "Mode SaveProgress";
const char s_ctt64[] PROGMEM = "Mode OneLargeTrack";

const char s_ctt70[] PROGMEM = "None";
const char s_ctt71[] PROGMEM = "PlPs";
const char s_ctt72[] PROGMEM = "PlPsLong";
const char s_ctt73[] PROGMEM = "PlPsDbl";
const char s_ctt74[] PROGMEM = "Next";
const char s_ctt75[] PROGMEM = "NextLong";
const char s_ctt76[] PROGMEM = "Prev";
const char s_ctt77[] PROGMEM = "PrevLong";

const char s_sep[] PROGMEM = ": ";

const char *const s_grTable[] PROGMEM = {s_gr0, s_gr1, s_gr2, s_gr3, s_gr4, s_gr5, s_gr6, s_gr7};
const char *const s_cttTable[] PROGMEM = {
    s_error, s_ctt00, s_ctt01, s_ctt02, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error,
    s_ctt10, s_ctt11, s_ctt12, s_ctt13, s_ctt14, s_ctt15, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error,
    s_ctt20, s_ctt21, s_ctt22, s_ctt23, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error,
    s_ctt30, s_ctt31, s_ctt32, s_ctt33, s_ctt34, s_ctt35, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error,
    s_ctt40, s_ctt41, s_ctt42, s_ctt43, s_ctt44, s_ctt45, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error,
    s_ctt35, s_ctt51, s_ctt52, s_ctt53, s_ctt54, s_ctt55, s_ctt56, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error,
    s_ctt60, s_ctt61, s_ctt62, s_ctt63, s_ctt64, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error, s_error,
    s_ctt70, s_ctt71, s_ctt72, s_ctt73, s_ctt74, s_ctt75, s_ctt76, s_ctt77, s_ctt35, s_ctt03, s_error, s_error, s_error, s_error, s_error, s_error};

const char *MessageToString::getStringFromMessage(const Message::eMessageContent msg)
{
    // SUCCESS: proven that the guards prevent segfault when called with nonexistant content.
    m_completeMessage[0] = 0; //delete string
    Message message{Message(msg)};
    uint8_t content = message.getContentInt();

    if(content == 0)
    {
        return nullptr; // no message. Won't print.
    }

    // return error message stored in s_ctttable[0]
    if((content > Message::LASTVALIDMESSAGE))
    {
        strcpy_P(m_completeMessage, (char *)(pgm_read_word(&(s_cttTable[0]))));
        return m_completeMessage;
    }

    strcpy_P(m_completeMessage, (char *)(pgm_read_word(&(s_grTable[message.getGroupIdInt()]))));
    strcat_P(m_completeMessage, s_sep);
    strcat_P(m_completeMessage, (char *)(pgm_read_word(&(s_cttTable[message.getContentInt()]))));
    return m_completeMessage;
}
#else
const char *MessageToString::getStringFromMessage(const Message::eMessageContent msg)
{
    return s_error;
}
#endif