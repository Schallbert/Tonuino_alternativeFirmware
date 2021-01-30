#ifndef ARDUINO_CONFIG_H
#define ARDUINO_CONFIG_H

// CPU clock Speed
//#define F_CPU 8000000UL
#define MS_TO_S 1000

//Pin defines ------------------------
#define PINANALOG_RNDMGEN 19 //ADC1 28 A5
// MFRC522
#define RST_PIN 9 // PB1 15
#define SS_PIN 10 // PB2 16
//MP3
#define DFMINI_PIN_ISIDLE 4 // PD4 7
#define DFMINI_RX 2         // PD2 5
#define DFMINI_TX 3         // PD3 6
                            //Serial
                            // RX 0  PD0 2
                            // TX 1  PD1 3
                            //HMI inputs

// USERINPUT VARIANT : CLICK_ENCODER
#define ENCA 14  // PC0 23 A0
#define ENCB 15  // PC1 24 A1
#define ENCSW 16 // PC2 24 A2
// USERINPUT VARIANT : THREE_BUTTONS
#define PINPLPS 14 // PC0 23 A0
#define PINPREV 16 // PC1 25 A2
#define PINNEXT 15 // PC2 24 A1

//KeepAlive
#define KEEPALIVE_PIN 17 //PC3 26 A3
#define KEEPALIVE_ACTIVE_STATE LOW
//Feedback LED
#define LED_PIN 18 //PC4 27 A4
#define LED_ACTIVE_STATE HIGH

#endif // ARDUINO_CONFIG_H