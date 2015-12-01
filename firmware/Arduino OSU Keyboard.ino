#include "UsbKeyboard.h"
#include <avr/interrupts.h>
#include <avr/wdt.h>

#define BT1_PIN A0
#define BT2_PIN A1
#define BT3_PIN A2

#define BT1_LED 6
#define BT2_LED 7

#define LED_R A3
#define LED_G A4
#define LED_B A5

#define BYPASS_PASSIVE_DELAY 3

#define BYPASS_TIMER_ISR 1

#define USB_ATTACH_FAIL_COUNT 10

#define ENABLE_DEBUG_OUTPUT

bool fore_KEY_BT1 = false;
double fore_KET_BT1_RUNTIME = 0;

bool fore_KEY_BT2 = false;
double fore_KET_BT2_RUNTIME = 0;

bool fore_KEY_BT3 = false;
double fore_KET_BT3_RUNTIME = 0;

int BT1_LED_ALPHA = 0;
int BT1_LED_ALPHA_TARGET = 0;
int BT2_LED_ALPHA = 0;
int BT2_LED_ALPHA_TARGET = 0;

int LED_TARGET_H = 0;
int LED_PRESENT_H = 0;

const int LED_U = 0;
const int LED_E = 0;




uchar reportBuffer[4];
/* Report Buffer Prefix
 reportBuffer[0]-> BT1 -> KEY_Z
 reportBuffer[1]-> BT2 -> KEY_X
 reportBuffer[2]-> BT3 -> KEY_ESC
 reportBuffer[3]-> NULL-> 0
 */


long loop_route_time;
void setup() {

  Serial.begin(115200);

  Serial.println("=========================================================");
  Serial.println("     Arduino OSU Keyboard    Designed by MartinT3CH      ");
  Serial.println("         GITHUBl https://github.com/MarsTechHAN          ");
  Serial.println("         Opensource under lincense GNU/GPL v3            ");
  Serial.println("         BUILD"__DATE__" "__TIME__"                      ");
  Serial.println("=========================================================");

#ifdef ENABLE_DEBUG_OUTPUT
  Serial.println("WARNING: DEBUG OUTPUT has been turned on, which may redu-");
  Serial.println("ce the performance! Disable #define ENABLE_DEBUG_OUTPUT !");
  Serial.println("");
  Serial.print(  "SYSTEM BOOTING.........................................");
#endif
  pinMode(BT1_PIN, INPUT_PULLUP);
  pinMode(BT2_PIN, INPUT_PULLUP);
  pinMode(BT3_PIN, INPUT_PULLUP);

  pinMode(BT1_LED , OUTPUT);
  pinMode(BT2_LED , OUTPUT);

  pinMode(LED_R , OUTPUT);
  pinMode(LED_G , OUTPUT);
  pinMode(LED_B , OUTPUT);

  analogWrite(BT1_LED, 255);
  analogWrite(BT2_LED, 255);

  analogWrite(LED_R, 255);
  analogWrite(LED_G, 255);
  analogWrite(LED_B, 255);



  loop_route_time = 0;
  memset(reportBuffer, 0, sizeof(reportBuffer));

#if BYPASS_TIMER_ISR
  // disable timer 0 overflow interrupt (used for millis)
  TIMSK0 &= !(1 << TOIE0); // ++
#endif

#ifdef ENABLE_DEBUG_OUTPUT
  Serial.println("OK");
  Serial.print("USB ATTACHED......................................");
  int fail_count = 0;
  while(!usbInterruptIsReady()){
    delay(1000);
    fail_count++;
    if(fail_count >= USB_ATTACH_FAIL_COUNT) {
      Serial.println("FAIL");
      Serial.println("CRITICAL ERROR: UNABLE TO ATACHED USB, REBOOT IN 3s");
      delay(3000);
      asm volatile ("jmp 0");
    }
  }
  Serial.println("OK");
  Serial.println();
#endif
}

#if BYPASS_TIMER_ISR
void delayMs(unsigned int ms) {
  /*
  */
  for (int i = 0; i < ms; i++) {
    delayMicroseconds(1000);
  }
}
#endif

bool KEY_CHANGE_REQUEST = false;

void loop() {

  ++loop_route_time;
  
  usbPoll;

  if (fore_KEY_BT1 == false && digitalRead(BT1_PIN) == false) {
    fore_KET_BT1_RUNTIME = loop_route_time;
    reportBuffer[3] = KEY_Z;
    fore_KEY_BT1 = true;
    KEY_CHANGE_REQUEST = true;
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.println("BT1_PRESSED_DOWN_EVENT TRIGGERED");
    Serial.println("reportBuffer 3 LOADED -> KEY_Z");
    Serial.print("BT1 EVENT LOG RUNTIME:"); 
    Serial.println(loop_route_time);
    Serial.println("USB STATUS UPDATA FLAG SET");
    Serial.println("");
#endif

  }

  if (fore_KEY_BT2 == false && digitalRead(BT2_PIN) == false) {
    fore_KET_BT2_RUNTIME = loop_route_time;
    reportBuffer[1] = KEY_X;
    fore_KEY_BT2 = true;
    KEY_CHANGE_REQUEST = true;

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.println("BT2_PRESSED_DOWN_EVENT TRIGGERED");
    Serial.println("reportBuffer 1 LOADED -> KEY_X");
    Serial.print("BT2 EVENT LOG RUNTIME:"); 
    Serial.println(loop_route_time);
    Serial.println("USB STATUS UPDATA FLAG SET");
    Serial.println("");
#endif

  }

  if (fore_KEY_BT3 == false && digitalRead(BT3_PIN) == false) {
    fore_KET_BT3_RUNTIME = loop_route_time;
    reportBuffer[2] = KEY_ESC;
    fore_KEY_BT3 = true;
    KEY_CHANGE_REQUEST = true;

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.println("BT3_PRESSED_DOWN_EVENT TRIGGERED");
    Serial.println("reportBuffer 2 LOADED -> KEY_ESC");
    Serial.print("BT3 EVENT LOG RUNTIME:"); 
    Serial.println(loop_route_time);
    Serial.println("USB STATUS UPDATA FLAG SET");
    Serial.println("");
#endif
  }



  if (fore_KEY_BT1 == true && digitalRead(BT1_PIN) == true) {
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.println("BT1_RELEASED_EVENT TRIGGERED");
    Serial.print("BYPASS_PASSIVE_LOOP :");
    Serial.println(abs(fore_KET_BT1_RUNTIME - loop_route_time));
#endif
    if (abs(fore_KET_BT1_RUNTIME - loop_route_time) > BYPASS_PASSIVE_DELAY) {
      reportBuffer[3] = 0;
      fore_KEY_BT1 = false;
      KEY_CHANGE_REQUEST = true;
#ifdef ENABLE_DEBUG_OUTPUT
      Serial.println("reportBuffer 3 LOADED -> KEY_NONE");
      Serial.println("USB STATUS UPDATA FLAG SET");
      Serial.println("");
#endif
    }
    else{
#ifdef ENABLE_DEBUG_OUTPUT
      Serial.println("BYPASS_PASSIVE_LOOP NOT REACH CRITICAL");
      Serial.println("EVENT INGORED");
      Serial.println("");
#endif
    }
  }

  if (fore_KEY_BT2 == true && digitalRead(BT2_PIN) == true) {
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.println("BT2_RELEASED_EVENT TRIGGERED");
    Serial.print("BYPASS_PASSIVE_LOOP :");
    Serial.println(abs(fore_KET_BT2_RUNTIME - loop_route_time));
#endif
    if (abs(fore_KET_BT2_RUNTIME - loop_route_time) > BYPASS_PASSIVE_DELAY) {
      reportBuffer[1] = 0;
      fore_KEY_BT2 = false;
      KEY_CHANGE_REQUEST = true;
#ifdef ENABLE_DEBUG_OUTPUT
      Serial.println("reportBuffer 1 LOADED -> KEY_NONE");
      Serial.println("USB STATUS UPDATA FLAG SET");
      Serial.println("");
#endif
    }
    else{
#ifdef ENABLE_DEBUG_OUTPUT
      Serial.println("BYPASS_PASSIVE_LOOP NOT REACH CRITICAL");
      Serial.println("EVENT INGORED");
      Serial.println("");
#endif
    }
  }

  if (fore_KEY_BT3 == true && digitalRead(BT3_PIN) == true) {
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.println("BT3_RELEASED_EVENT TRIGGERED");
    Serial.print("BYPASS_PASSIVE_LOOP :");
    Serial.println(abs(fore_KET_BT3_RUNTIME - loop_route_time));
#endif
    if (abs(fore_KET_BT1_RUNTIME - loop_route_time) > BYPASS_PASSIVE_DELAY) {
      reportBuffer[2] = 0;
      fore_KEY_BT3 = false;
      KEY_CHANGE_REQUEST = true;
#ifdef ENABLE_DEBUG_OUTPUT
      Serial.println("reportBuffer 2 LOADED -> KEY_NONE");
      Serial.println("USB STATUS UPDATA FLAG SET");
      Serial.println("");
#endif
    }
    else{
#ifdef ENABLE_DEBUG_OUTPUT
      Serial.println("BYPASS_PASSIVE_LOOP NOT REACH CRITICAL");
      Serial.println("EVENT INGORED");
      Serial.println("");
#endif
    }
  }

  if (KEY_CHANGE_REQUEST) {
    
    if (!usbInterruptIsReady()) 
    {
#ifdef ENABLE_DEBUG_OUTPUT
      Serial.println("USB INTERRUPT NOT READY YET, LOOP!");
#endif
      return;
    }
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
    KEY_CHANGE_REQUEST = false;
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.println("DATA PACKAGED READY TO SEND, WAIT FOR INTERRUPT");
    Serial.println("USB STATUS UPDATA FLAG CLEAR");
    Serial.println("");
#endif
  }

#if BYPASS_TIMER_ISR  // check if timer isr fixed.
  delayMs(20);
#else
  delay(20);
#endif

}

