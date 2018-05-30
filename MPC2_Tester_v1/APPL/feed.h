#ifndef FEEDING_INCLUDED
#define FEEDING_INCLUDED  1

#include "../lib/boolean.h"
#include "../init/start.h"

enum {
  FD_INIT = 0,
  FD_WAIT_IX_LOW_START_STAND_ALONE,
  FD_WAIT_IX_HIGH_START_STAND_ALONE,
  FD_WAIT_EF_START_STAND_ALONE,
  FD_WAIT_START_COMPUTERIZED,
  FD_WAIT_FEED_START,
  FD_CHECK_BALANCE,
  FD_FEEDING,
  FD_WAIT_INTERVAL_TIME
  };
typedef unsigned char FEED_STATE;

//extern FEED_STATE      FeedState[MAX_NODES];

extern BOOLEAN         FEEDING_ENABLE[MAX_NODES];
extern non_banked void HndlFeeding_Non_Banked(void); 

void InitFeed(void);
void HndlFeeding( unsigned char side );

#endif
