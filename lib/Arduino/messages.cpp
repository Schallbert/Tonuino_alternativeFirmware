#include <avr/pgmspace.h>

#include "MessageHandler_interface.h"
#include "messages.h"

// WARNING:
// Custom Message s_group + ": " + s_content may not count
// more letters than DEBUG_MAXMESSAGELENGTH -1 or there will be SEGFAULTs!
#if DEBUGSERIAL
const char s_group0[] PROGMEM = "System";
const char s_group1[] PROGMEM = "Mp3Control";
const char s_group2[] PROGMEM = "Mp3Playback";
const char s_group3[] PROGMEM = "Mp3Player";
const char s_group4[] PROGMEM = "NfcControl";
const char s_group5[] PROGMEM = "NfcReader";
const char s_group6[] PROGMEM = "Playlist";

const char s_content0[] PROGMEM = "Started";
const char s_content1[] PROGMEM = "Shutdown";
const char s_content2[] PROGMEM = "Prompt timeout";

const char s_content3[] PROGMEM = "Volume up";
const char s_content4[] PROGMEM = "Volume down";
const char s_content5[] PROGMEM = "Next track";
const char s_content6[] PROGMEM = "Previous track";
const char s_content7[] PROGMEM = "Play";
const char s_content8[] PROGMEM = "Pause";

const char s_content9[] PROGMEM = "Error: folder invalid!";
const char s_content10[] PROGMEM = "Folder detected. Play";
const char s_content11[] PROGMEM = "Autoplay complete. Pause";
const char s_content12[] PROGMEM = "Autoplay next";

const char s_content13[] PROGMEM = "Finished playing track";
const char s_content14[] PROGMEM = "SD card online";
const char s_content15[] PROGMEM = "SD card inserted";
const char s_content16[] PROGMEM = "SD Card removed";
const char s_content17[] PROGMEM = "Com Error";

const char s_content18[] PROGMEM = "No Tag";
const char s_content19[] PROGMEM = "Active Tag";
const char s_content20[] PROGMEM = "New Tag";
const char s_content21[] PROGMEM = "Unknown Tag";

const char s_content22[] PROGMEM = "Tag Reader Init";
const char s_content23[] PROGMEM = "Tag Write Success";
const char s_content24[] PROGMEM = "Tag Read Success";
const char s_content25[] PROGMEM = "Tag write Error";
const char s_content26[] PROGMEM = "Tag read Error";
const char s_content27[] PROGMEM = "Tag type not implemented";

const char s_content28[] PROGMEM = "Mode Lullabye";
const char s_content29[] PROGMEM = "Mode Album";
const char s_content30[] PROGMEM = "Mode Random";
const char s_content31[] PROGMEM = "Mode SaveProgress";
const char s_content32[] PROGMEM = "Mode OneLargeTrack";

const char *const s_groupTable[] PROGMEM = {s_group0, s_group1, s_group2, s_group3, s_group4, s_group5, s_group6};
const char *const s_contentTable[] PROGMEM = {s_content0, s_content1, s_content2, s_content3, s_content4,
                                              s_content5, s_content6, s_content7, s_content8, s_content9,
                                              s_content10, s_content11, s_content12, s_content13, s_content14,
                                              s_content15, s_content16, s_content17, s_content18, s_content19,
                                              s_content20, s_content21, s_content22, s_content23, s_content24,
                                              s_content25, s_content26, s_content27, s_content28, s_content29,
                                              s_content30, s_content31, s_content32};

char *MessageToString::getStringFromMessage(const Message &message)
{
    m_completeMessage[0] = 0; //delete string
    strcpy_P(m_completeMessage, (char *)(pgm_read_word(&(s_groupTable[message.m_group]))));
    strcat_P(m_completeMessage, ": ");
    strcat_P(m_completeMessage, (char *)(pgm_read_word(&(s_contentTable[message.m_contents]))));
    return m_completeMessage;
}
#else
char *MessageToString::getStringFromMessage(Message message)
{
    return nullptr;
}
#endif