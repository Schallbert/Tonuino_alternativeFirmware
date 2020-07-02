#ifndef DEFINES_H
#define DEFINES_H

//#define F_CPU 8000000UL

//DEBUG OUTPUT [just comment out this define to disable all serial outputs]
#define DEBUGSERIAL false

//mp3 Messages -----------------------
#define MSG_DELETETAG 800
#define MSG_ABORTEED 802
#define MSG_ERROR 401
#define MSG_TAGCONFSUCCESS 400
#define MSG_UNKNOWNTAG 300
#define MSG_TAGLINKED 310
#define MSG_SELECTFILE 320
#define MSG_HELP 330

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
#define KEEPALIVE A0 //PB6 9
//Feedback LED
#define LED_PIN 13 //for test purposes only, otherwise 16 //PC0 25

//KeepAlive interaction outputs
#define MAXIDLE 20 //Seconds
#define FLASHSLOWMS 500
#define FLASHQUICKMS 200

#define VOLUME_MAX 22
#define VOLUME_MIN 2
#define VOLUME_INIT 7
#define DFMINI_EQ_SETTING DfMp3_Eq::DfMp3_Eq_Normal

//Timer1 interrupt task
#define TIMERONE_TASK_INTERVAL_USEC 1000 // 1ms
#define LULLABYE_TIMER_SECS 20*60 // default is 20 minutes

//MP3 defines
#define WAIT_DFMINI_READY 1000 // 1 sec

#endif