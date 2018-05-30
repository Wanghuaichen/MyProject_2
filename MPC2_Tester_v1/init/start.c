/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*                                                              */
/*Description: Start applications                               */
/*                                                              */
/*                                                              */
/****************************************************************/

/* include files                                                */
#pragma codeseg(EEPAGE)
                             
#include "../lib/taskdef.h"
#include "../lib/schedule.h"
#include "../lib/rti_int.h"
#include "../lib/typedef.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/extport.h"
#include "../lib/watchdog.h"
#include "../include/romeehc1.h"
#include "../include/romstnet.h"
#include "../flowlib/peripheral_vcp_flow.h"
#include "../comm/send_msg.h"
#include "../comm/flowctrl.h"
#include "../key/key.h"
#include "../key/disp.h"
#include "../net/ext_net.h"
#include "../lib/stdlib.h"
#include "start.h"
                   
extern unsigned char msgTimeoutTime; 

PerGenRef *p_PerGenRef[MAX_NODES];
unsigned short PerGenNumber[MAX_NODES];

unsigned char HndlPerGenNumberRefReq(signed short PerAddress);
unsigned char HndlPerGenRefReq(unsigned short PerNumber, signed short PerAddress);
 

/****************************************************************/
/* Start Program                                                */
/****************************************************************/

void Start(void) 
{
  unsigned char done, first;
  unsigned long Time=0, NextTime;

  if(p_PerGenRef[0] != NULL ){free(p_PerGenRef[0]); p_PerGenRef[0] = NULL;}
  if(p_PerGenRef[1] != NULL ){free(p_PerGenRef[1]); p_PerGenRef[1] = NULL;}
  
  TaskStart(NET_REC_TASK);
  TaskStart(NET_SEND_CONTROL_TASK);
  
  TaskStart(KEY_CONTROL_TASK);
  
  //check first PerGenNumber

  PerGenNumber[0] = 0;
  PerGenNumber[1] = 0;
 
  if (EEAddressCopy != 0) {
    done = 0; first = 1; NextTime = 0;
    while (!done) {
      WatchDog();
      Time = Cnt1sec;
      if (PerGenNumber[0] == 0) {
        if (Time > NextTime) {
          NextTime = Cnt1sec + msgTimeoutTime != 5 ? 1 : 15;
          HndlPerGenNumberRefReq(EEAddressCopy);
          // doorstarten als Reinigings toets ingedrukt is
          if (KeyMode == KM_CLEANING)  {
            NetClose(CHANNEL);
            done = 1;
            EEAddressCopy = 0;
          }
        }    
      } else {
        if (first) {NextTime = 0; first = 0;}
        if (p_PerGenRef[0] == NULL) {
          if (Time > NextTime) {
            if (HndlPerGenRefReq(PerGenNumber[0], EEAddressCopy)){
              NextTime = Cnt1sec + 15;
              while(HndlHerdConRefReq() == 0){ TaskExit(); WatchDog(); }
            }  
          }
        } else {
          done = 1;
        }
      }
      TaskExit();
    }

    //check second  PerGenNumber
    if (EEAddress2 != 0 && EEAddressCopy != 0) {
      PerGenNumber[1] = 0;
      done = 0; first = 1; NextTime = 0;
      while (!done) {
        WatchDog();
        Time = Cnt1sec;
        if (PerGenNumber[1] == 0) {
          if (Time > NextTime) {
            NextTime = Cnt1sec + 15;
            HndlPerGenNumberRefReq(EEAddress2);
          }
        } else {
          if (first) {NextTime = 0; first = 0;}
          if (p_PerGenRef[1] == NULL) {
            if (Time > NextTime) {
              NextTime = Cnt1sec + 15;
              HndlPerGenRefReq(PerGenNumber[1], EEAddress2);
            }
          } else {
            done = 1;
          }
        }
      }
    }
  }
  
  KeyMode = KM_CLEANING;
  ScreenMode = SM_SERVICE;

  TaskStart(MILK_CONTROL_TASK);
  if ( EEFeeding && ( SoftKey & SK_FEEDING_ENABLE)) {
  	TaskStart(FEED_A_CONTROL_TASK);
    
    // start tweede voerprocess igv swing-over
    if ( EEAddress2 )TaskStart(FEED_B_CONTROL_TASK);
  }
   
  // open-tandem control, deze taak wordt alleen opgestart indien de desbetreffende 
  // PIC aanwezig is, alsmede de juiste instellingen gedaan zijn
  if ( (SoftKey & SK_TANDEM_ENABLE ) && 
       ((ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) && (ParlourType >= PT_SEMI_TANDEM)) ){
    TaskStart(TANDEM_CONTROL_TASK);
  }
  TaskStop(START_TASK);
}

 

