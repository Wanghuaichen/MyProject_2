#include "../lib/io6811.h"
#include "../lib/rti_int.h"
#include "../lib/boolean.h"
#include "../lib/stdlib.h"
#include "../lib/eehc11.h"
#include "../lib/typedef.h"
#include "../lib/extport.h"
#include "../lib/mem.h"
#include "../lib/iic.h"
#include "../lib/time.h"
#include "../lib/item.h"
#include "../lib/schedule.h"
#include "../lib/taskdef.h"
#include "../motor/motor.h"
#include "../usx/ext_usx.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"
#include "../appl/chkerr.h"
#include "../appl/feed.h"
#include "../key/key.h"
#include "../key/disp.h"
#include "../key/keyint.h"
#include "../init/start.h"
#include "../comm/flowctrl.h"
#include "../flowlib/feed_vcp_flow.h"


FEED_STATE      FeedState[MAX_NODES];
BOOLEAN         FEEDING_ENABLE[MAX_NODES];

// =============================================================================
// Initeer voer process
// =============================================================================
void InitFeed(void)
{
#if ( _HP_TEST_ == 0 )

  FeedState[0] = FeedState[1] = FD_INIT;
  ExtraFeed[0] = ExtraFeed[1] = 0;

  if (p_CowFedParlRes[0]  != NULL) { free(p_CowFedParlRes[0] ); p_CowFedParlRes[0]  = NULL; }
  if (p_CowFedParlRes[1]  != NULL) { free(p_CowFedParlRes[1] ); p_CowFedParlRes[1]  = NULL; }
  if (p_CowFedParlRef[0]  != NULL) { free(p_CowFedParlRef[0] ); p_CowFedParlRef[0]  = NULL; }
  if (p_CowFedParlRef[1]  != NULL) { free(p_CowFedParlRef[1] ); p_CowFedParlRef[1]  = NULL; }

#endif
}

// =============================================================================
void HndlFeeding( unsigned char side )
{
#if ( _HP_TEST_ == 0 )

  unsigned short IntervalTime;
  unsigned short TotalPortions = 0;
  unsigned short mtrportion, speed;
    signed short balance;
  unsigned short time, x;
  unsigned char  temp;

  while(1)
  {

    if ( EEDummy1 == 0){
      // taak hoeft alleen actief te zijn tijdens melken
      switch( FeedState[side] ){
        case FD_INIT:                                // STATE 0
             if (p_CowFedParlRes[side]  != NULL) { free(p_CowFedParlRes[side] ); p_CowFedParlRes[side]  = NULL; }

             while ( !((KeyMode == KM_MILKING) || (KeyMode == KM_RBT_MILKING)) ) TaskExit();
             if ( EEAddressCopy == 0 ) {
               if (EEFeedBalanceDef[side] ) FeedState[side] = FD_WAIT_IX_LOW_START_STAND_ALONE;
               else                         FeedState[side] = FD_WAIT_EF_START_STAND_ALONE;
             } else {
               FeedState[side] = FD_WAIT_START_COMPUTERIZED;
             }
          break;

        case FD_WAIT_IX_LOW_START_STAND_ALONE:             // STATE 1
             // stand-alone mode met default values, display ??
             // als I3 actief wordt display default en start met voeren als I3 weer hoog wordt
             if ( (side == SIDE_A && ( ReadI3Sensor() == SENSOR_STATE_CLOSE )) ||
                  (side == SIDE_B && ( ReadI4Sensor() == SENSOR_STATE_CLOSE )) ){
               // initieer pointer en start voeren
               FeedState[side] = FD_WAIT_IX_HIGH_START_STAND_ALONE;
               ExtraFeed[side] = 0;

               while ( p_CowFedParlRef[side] == NULL) { p_CowFedParlRef[side] = calloc(1, sizeof(CowFedParlRef)); }
               if ( LONG_COW_GEN_NUMBER ){
                 (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance = EEFeedBalanceDef[side];
                 (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed       = EEFeedSpeed;
               } else {
                 (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance = EEFeedBalanceDef[side];
                 (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed       = EEFeedSpeed;
               }
               // maak de gevoerd pointer NULL
               if (p_CowFedParlRes[side]  != NULL) { free(p_CowFedParlRes[side] ); p_CowFedParlRes[side]  = NULL; }

             } else TaskExit();
          break;
        case FD_WAIT_IX_HIGH_START_STAND_ALONE:             // STATE 2
             if ( (side == SIDE_A && ( ReadI3Sensor() == SENSOR_STATE_OPEN )) ||
                  (side == SIDE_B && ( ReadI4Sensor() == SENSOR_STATE_OPEN )) ){
               // start afhankelijk van EEFeeding ivm motor-stromen
               time = Cnt10msec + ((EEFeeding * 3 * NR_OF_10MS_SEC)/2 );
               FeedState[side] = FD_WAIT_FEED_START;
             } else TaskExit();

          break;
        case FD_WAIT_EF_START_STAND_ALONE:                   // STATE 3
             // wacht met voeren tot handmatig een portie is ingegeven
             if ( ExtraFeed[side] ){
               while ( p_CowFedParlRef[side] == NULL) { p_CowFedParlRef[side] = calloc(1, sizeof(CowFedParlRef)); }

               if ( LONG_COW_GEN_NUMBER ){
                 (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed = EEFeedSpeed;

                 balance = ((((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance)+ ExtraFeed[side];
                 if ( p_CowFedParlRes[side] != NULL ) balance -= (((ExtCowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed;
               } else {
                 (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed = EEFeedSpeed;

                 balance = ((((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance)+ ExtraFeed[side];
                 if ( p_CowFedParlRes[side] != NULL ) balance -= (((CowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed;
               }
               if ( balance ){
                 // start afhankelijk van EEFeeding ivm motor-stromen
                 time = Cnt10msec;
                 FeedState[side] = FD_WAIT_FEED_START;
               } else TaskExit();
             } else TaskExit();
          break;

        case FD_WAIT_START_COMPUTERIZED:                     // STATE 4
             if ( p_CowFedParlRef[side] != NULL ) {
               FeedState[side] = FD_WAIT_FEED_START;
             } else {
               if ( ExtraFeed[side] ){
                 // handmatig een voertegoed ingevuld, echter geen pointer aanwezig
                 // creeer een pointer, ook evt de PerGenRefPtr
                 while (p_CowFedParlRef[side] == NULL) {p_CowFedParlRef[side] = calloc(1, sizeof(CowFedParlRef));}
                 if ( LONG_COW_GEN_NUMBER ){
                   (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance = 0;
                   (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed       = EEFeedSpeed;
                 } else {
                   (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance = 0;
                   (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed       = EEFeedSpeed;
                 }
                 // kijk ook of er PerGenRef record is, zoniet creer er een
                 if (p_PerGenRef[side] == NULL) {
                   // alloceer geheugen
                   while (p_PerGenRef[side] == NULL) {p_PerGenRef[side] = calloc(1, sizeof(PerGenRef));}
                   (p_PerGenRef[side]->Array)[0].MtrGenPortion = EEFeedMtrPortion[side];
                 }

                 // maak de gevoerd pointer NULL
                 if (p_CowFedParlRes[side] != NULL) { free(p_CowFedParlRes[side] ); p_CowFedParlRes[side]  = NULL; }
               }
               TaskExit();
             }
             // FeedStartDelayTime, enable feeding
             FEEDING_ENABLE[side] = TRUE;

             time = Cnt10msec + ((p_PerGenRef[side]->PerGenBlock9)*NR_OF_10MS_SEC);
          break;

        case FD_WAIT_FEED_START:                             // STATE 5
             // wacht op start
             while( !TimeControl( Cnt10msec, time )) TaskExit();
             if (FeedState[side] == FD_WAIT_FEED_START )FeedState[side] = FD_CHECK_BALANCE;
          break;

        case FD_CHECK_BALANCE:                               // STATE 6
             // pointer dienen ongelijk NULL te zijn
             if ( p_CowFedParlRef[side] == NULL  ||
                ( !((KeyMode == KM_MILKING) || (KeyMode == KM_RBT_MILKING) || (KeyMode == KM_INPUT) || KeyMode == KM_DISP_MSG) )){
               FeedState[side] = FD_INIT;
             } else {
                  // wacht indien melken gestopt, kan weer gestart worden als melkstel weer
                  // wordt ondergehangen
               while ( EEAddressCopy && !(FEEDING_ENABLE[side]) && p_CowFedParlRef[side] != NULL ) TaskExit();

               if ( p_CowFedParlRef[side] == NULL ){
                 FeedState[side] = FD_INIT;
               } else {
                 if ( LONG_COW_GEN_NUMBER ){
                   balance = ((((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance)+ ExtraFeed[side];
                   speed   =  (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed;
                   if ( p_CowFedParlRes[side] != NULL ) balance -= (((ExtCowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed;
                 } else {
                   balance = ((((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance)+ ExtraFeed[side];
                   speed   =  (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].GrpLmtSpeed;
                   if ( p_CowFedParlRes[side] != NULL ) balance -= (((CowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed;
                 }

                 // aantal porties opnieuw berekenen
                 TotalPortions = 0;

                 if ( (balance > 0) && (p_CowFedParlRef[side] != NULL )){
                   mtrportion = (EEAddressCopy == 0 ? EEFeedMtrPortion[side] : (p_PerGenRef[side]->Array)[0].MtrGenPortion);
                   if ( mtrportion ){
                     TotalPortions = balance / mtrportion;
                     // indien restvoer 1 portie meer voeren
                     if ( ((balance % mtrportion) * 2) >= mtrportion ) TotalPortions++;
                     IntervalTime = (( mtrportion * 60 ) / (speed != 0 ? speed : 300) );
                   }
                 }

                 if ( TotalPortions ) {
                   FeedState[side]  = FD_FEEDING;
                 } else {
                   // wacht op eventueel voertegoed bv handmatig, of herstart voercyclus
                   if ( !EEAddressCopy ){
                     if( !EEFeedBalanceDef[side] ) FeedState[side] = FD_WAIT_EF_START_STAND_ALONE;
                     else {
                       if ( side == SIDE_A ){
                         if( ReadI3Sensor() == SENSOR_STATE_CLOSE ) FeedState[SIDE_A] = FD_WAIT_IX_LOW_START_STAND_ALONE;
                       } else {
                         if( ReadI4Sensor() == SENSOR_STATE_CLOSE ) FeedState[SIDE_B] = FD_WAIT_IX_LOW_START_STAND_ALONE;
                       }
                     }
                   } else {
                     if ( ( GetCowAtt(side) & (CA_SEP+CA_COLOST+CA_DRY_BLCK)))  {
                       // deze koeien worden niet gemolken maar wel gevoerd, dus afboeken
                       if ( LONG_COW_GEN_NUMBER ? ((ExtMlkRef **)p_MlkRef)[side]->CowGenNumber : ((MlkRef **)p_MlkRef)[side]->CowGenNumber ){
                         HndlCowFedParlRes(side);
                         if (p_CowFedParlRes[side]  != NULL) { free(p_CowFedParlRes[side] ); p_CowFedParlRes[side]  = NULL; }
                         if (p_CowFedParlRef[side]  != NULL) { free(p_CowFedParlRef[side] ); p_CowFedParlRef[side]  = NULL; }
                         ExtraFeed[side] = 0;
                         FeedState[side] = FD_INIT;
                       }
                     }
                   }
                 }
                 TaskExit();
               }
             }
          break;

        case FD_FEEDING:                                     // STATE 7
             if ( !CheckErrors() ){
               // afhankelijk van de relais-tijd wordt er een motor of een relais aangestuurd
               if ( side == SIDE_A ){
                 if (EERelaisTime == 0){
                   temp =  MotorControl( 0, TURN );
                  } else {
                   // relais aansturing
                   temp = RelaisControl();
                 }
               } else {
                 if ( side == SIDE_B ){
                   // B-zijde kan alleen via relais worden aangestuurd
                   if ( EESB_RelaisTime ){
                     ExtPortK3State = EPK3S_ON;
                     if ( x = (EESB_RelaisTime % 10 )){
                       // milli seconden
                       time = TCNT + (x*2000);
                       while( !TimeControl( TCNT, time));
                     }
                     if ( x = (EESB_RelaisTime / 10 )){
                       // 10_milli seconden
                       time = Cnt10msec + x;
                       while( !TimeControl( Cnt10msec, time))TaskExit();
                     }
                     ExtPortK3State = EPK3S_OFF;
                     temp = MOTOR_TURNED;
                   } else {
                     // niet gevoerd
                     temp = MOTOR_FAILURE;
                   }
                 }
               }
               if ( temp == MOTOR_TURNED || temp == MOTOR_INPUT_CONTACT_ERROR  || temp == MOTOR_CONF_ERROR ){
                 // reserveer eventueel ruimte voor het resultaat
                 while(p_CowFedParlRes[side] == NULL){
                   p_CowFedParlRes[side] = calloc(1, sizeof(CowFedParlRes));
                 }

                 if (LONG_COW_GEN_NUMBER ){
                   (((ExtCowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed
                           += (EEAddressCopy == 0 ? EEFeedMtrPortion[side] : ((p_PerGenRef[side]->Array)[0].MtrGenPortion));
                 } else {
                   (((   CowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed
                     	+= (EEAddressCopy == 0 ? EEFeedMtrPortion[side] : ((p_PerGenRef[side]->Array)[0].MtrGenPortion));
                 }
                 TotalPortions--;
               }
               time = Cnt10msec + (IntervalTime*NR_OF_10MS_SEC);
             } else {
               time = Cnt10msec + (5*NR_OF_10MS_SEC);
             }

             if (FeedState[side] ==  FD_FEEDING) FeedState[side] = FD_WAIT_INTERVAL_TIME;
          break;

        case FD_WAIT_INTERVAL_TIME:                          // STATE 8
             // FeedState kan gewijzig worden in HandlKey
             while( !TimeControl( Cnt10msec, time ) && (FeedState[side] == FD_WAIT_INTERVAL_TIME) ){

               if ( !EEAddressCopy ){
                 // StandAlone
                 if ( side == SIDE_A ){
                   if ( ReadI3Sensor() == SENSOR_STATE_CLOSE ) {
                     FeedState[SIDE_A] = FD_WAIT_IX_LOW_START_STAND_ALONE;
                     break;
                   }
                 } else {
                   if ( ReadI4Sensor() == SENSOR_STATE_CLOSE ) {
                     FeedState[SIDE_B] = FD_WAIT_IX_LOW_START_STAND_ALONE;
                     break;
                   }
                 }
               }
               TaskExit();
             }

             if ( FeedState[side] != FD_WAIT_IX_LOW_START_STAND_ALONE) FeedState[side] = FD_CHECK_BALANCE;
          break;
        default: FeedState[side] = FD_INIT;
          break;

      } // end switch (FeedState )
    }
    TaskExit();                                    // geef andere taken een beurt
  } // end while(1)

#endif
}




