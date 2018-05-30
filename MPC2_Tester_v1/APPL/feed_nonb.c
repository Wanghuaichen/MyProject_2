#include "../appl/milkmisc.h"
#include "feed.h"



#pragma codeseg(RCODE)

// =============================================================================
// HndlFeeding                                           
// =============================================================================
non_banked void HndlFeeding_Non_Banked(void) 
{
  HndlFeeding(SIDE_A);
}


non_banked void HndlFeedingB_Non_Banked(void) 
{
  HndlFeeding(SIDE_B);
}
