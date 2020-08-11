#ifndef DEFINES_H
#define DEFINES_H
#include <DFMiniMp3_interface.h>

//#define F_CPU 8000000UL

//DEBUG OUTPUT [just write "false" to disable debug output on serial]
#define DEBUGSERIAL false

//mp3 Messages -----------------------
#define MSG_DELETETAG 800
#define MSG_ABORTEED 802
#define MSG_ERROR 401
#define MSG_ERROR_CARDREAD 402
#define MSG_ERROR_FOLDER 403
#define MSG_TAGCONFSUCCESS 400
#define MSG_UNKNOWNTAG 300 // TODO: rename o "select folderId?"
#define MSG_TAGLINKED 310 // TODO: Rename to "select playMode"
#define MSG_SELECTFILE 320
#define MSG_HELP 330
#define MSG_CONFIRMED 340

//Pin defines ------------------------
#define PINANALOG_RNDMGEN A1 //ADC1 24
// MFRC522
#define RST_PIN 9 // PB1 15
#define SS_PIN 10 // PB2 16
//MP3
#define DFMINI_BUSY 4 // PD4 6
#define DFMINI_RX 2   // PD2 4
#define DFMINI_TX 3   // PD3 5
//Serial
// RX 0  PD0 2
// TX 1  PD1 3
//HMI inputs
//   #define ENCA 18 // PC5 27
//   #define ENCB 17 // PC4 26
//   #define ENCSW 19 // PC6 28
#define PINPLPS 18 // PC5 27
#define PINPREV 17 // PC4 26
#define PINNEXT 19 // PC6 28
//KeepAlive
#define KEEPALIVE_PIN A0 //PB6 9
#define KEEPALIVE_ACTIVE_STATE LOW
//Feedback LED
#define LED_PIN 13 //for test purposes only, otherwise 16 //PC0 25
#define LED_ACTIVE_STATE HIGH

//StatusLEDs interaction outputs
#define FLASHSLOWMS 500
#define FLASHQUICKMS 200

//Folder specific data
#define MAXFOLDERCOUNT 99 // up to 255 supported by player

#define VOLUME_MAX 22 // Absolute max: 30
#define VOLUME_MIN 2 // Absolute min: 0
#define VOLUME_INIT 7 // Start volume. Anything between 0 and 30.
#define DFMINI_EQ_SETTING DfMiniMp3_interface::DfMp3_Eq_Normal // TODO How to solve for enum belonging to foreign file?

//Timer1 interrupt task
#define TIMERONE_TASK_INTERVAL_USEC 1000 // 1ms
#define IDLE_TIMEOUT_SECS 60 // System will automatically switch off when paused
#define LULLABYE_TIMEOUT_SECS 20*60 // default is 20 minutes (effective when track finished)
#define MENU_TIMEOUT_SECS 20 // menu will be automatically aborted if no input within this time

//MP3 defines
#define WAIT_DFMINI_READY 1000 // 1 sec
#define TIMEOUT_PROMPT_PLAYED 10000 // 10 sec

#endif