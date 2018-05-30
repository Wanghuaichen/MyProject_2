#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/typedef.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/rti_int.h"
#include "../lib/extport.h"
#include "../comm/flowctrl.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"


void HandleScreenRobot(void)
{
  unsigned short  DummyValue = 0;
  unsigned char   idx;

  idx = GetDisplaySide();

  switch (ScreenNumber) {
    case SCREEN_MILK_0:
         // display respondernummer op eerste vijf posities
         // moet hier blijven staan ivm punt in melk-gift
         // zet eerst allemaal spaties in de string
         DispValue((unsigned char *)&DummyValue, _U2, 1, 0, WRITE, POS_RIGHT);
         if (p_MlkRef[idx] != NULL) {
           DispValue(LONG_COW_GEN_NUMBER ?
                    (unsigned char *)&((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber :
                    (unsigned char *)&((   MlkRef **)p_MlkRef)[idx]->CowGenNumber, _U2, 5, 0, WRITE, POS_LEFT);
         } else {
           DispValue((unsigned char *)&DummyValue, _U2, 5, 0, WRITE, POS_LEFT);
         }

         // tbv min tekens
         DispTable[5] = DispTable[6] = ' ';

         // Show milk-yield
         if (MlkAmountKg >= 25) {
           DispValue((unsigned char *)&MlkAmountKg, _U2, 3, 1, READ_ONLY, POS_8);
         } else {
           DispValue((unsigned char *)&DummyValue, _U2, 3, 1, READ_ONLY, POS_8);
         }
      break;
  }
}