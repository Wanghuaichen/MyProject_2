/*******************************************************************/
/*                                                                 */
/*    AMICE 4.1  net_fase                                          */
/*                                                                 */
/*    Fifo and Tx pool operations on Tx and Rx ID's.               */
/*                                                                 */
/*******************************************************************/

#include "net_def.h"
#include "local_net.h"
#include "../lib/typedef.h"
#include "int_change.h"


#pragma codeseg(EEPAGE)

#define UPDATE_FIFO(fifo_ptr,item_ptr)    if ((++((fifo_ptr)->item_ptr)) >= (fifo_ptr)->End) \
                                            (fifo_ptr)->item_ptr = (fifo_ptr)->Beg

#define GET_FROM_FIFO(fifo_ptr, value)    value = *((fifo_ptr)->Rdp); \
                                          UPDATE_FIFO((fifo_ptr), Rdp)

#define PUT_IN_FIFO(fifo_ptr, value)      *((fifo_ptr)->Wrp) = (value); \
                                          UPDATE_FIFO((fifo_ptr), Wrp)

#define NOT_EMPTY_FIFO(fifo_ptr)          ((fifo_ptr)->Rdp != (fifo_ptr)->Wrp)


/*******************************************************************/
/*  GetFreeRxId                                                    */
/*  Get Rx ID from free-ID-fifo and return ID pointer.             */
/*  On low priority leave at least 1 ID in fifo.                   */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*                                                                 */
/*  Returnvalue:    == NULL - no ID available                      */
/*                  != NULL - pointer to Rx ID                     */
/*******************************************************************/

RxIdType *GetFreeRxId(NetType *NetPntr)
{
  RxIdType *ReturnRxIdPntr;
  RxCbType *RxCbPntr = &(NetPntr->RxCbId);

  /* check if any RxId's are free -----------------------------------------------------------------------------*/
  if (NOT_EMPTY_FIFO(RxCbPntr)) {

    /* determine priority -------------------------------------------------------------------------------------*/
    if ((NetPntr->RxDrCb).Priority == Low) {

      /* LOW priority request ---------------------------------------------------------------------------------*/

      /* keep at least 1 RxId free for high priority requests -------------------------------------------------*/
      if ((RxCbPntr->Rdp + 1 >= RxCbPntr->End ? RxCbPntr->Beg : RxCbPntr->Rdp + 1) == RxCbPntr->Wrp) {

        /* not enough RxId's free -----------------------------------------------------------------------------*/
        return (RxIdType *)NULL;
      }
    }

    /* get RxIdPntr from fifo ---------------------------------------------------------------------------------*/
    GET_FROM_FIFO(RxCbPntr, ReturnRxIdPntr);

    return ReturnRxIdPntr;
  }

  return (RxIdType *)NULL;
}


/*******************************************************************/
/*  HelpPutFreeRxId                                                */
/*  Put Rx ID into free-ID-fifo                                    */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*               *RxIdPntr  - pointer to Rx ID                     */
/*                                                                 */
/*******************************************************************/

void PutFreeRxId(NetType *NetPntr, RxIdType *RxIdPntr)
{
  RxCbType *RxCbPntr = &(NetPntr->RxCbId);
  char      old_intlevel;

  INTS_OFF(&old_intlevel);

  /* put RxIdPntr in fifo -------------------------------------------------------------------------------------*/
  PUT_IN_FIFO(RxCbPntr, RxIdPntr);

  INTS_ON(&old_intlevel);
}



/*******************************************************************/
/*  HelpPutFreeRxId                                                */
/*  Put Rx ID into free-ID-fifo                                    */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*               *RxIdPntr  - pointer to Rx ID                     */
/*                                                                 */
/*******************************************************************/

void IntPutFreeRxId(NetType *NetPntr, RxIdType *RxIdPntr)
{
  RxCbType *RxCbPntr = &(NetPntr->RxCbId);
/*char      old_intlevel;   */

/*INTS_OFF(&old_intlevel); */

  /* put RxIdPntr in fifo -------------------------------------------------------------------------------------*/
  PUT_IN_FIFO(RxCbPntr, RxIdPntr);

/*INTS_ON(&old_intlevel); */
}



/*******************************************************************/
/*  GetUsedRxId                                                    */
/*  Get Rx ID from low or high priority fifo.                      */
/*                                                                 */
/*  Parameter:   *RxCbPntr  - pointer to fifo                      */
/*                                                                 */
/*******************************************************************/

RxIdType *GetUsedRxId(RxCbType *RxCbPntr)
{
  RxIdType *ReturnRxIdPntr = (RxIdType *)NULL;

  if (NOT_EMPTY_FIFO(RxCbPntr)) {

    /* get RxIdPntr from fifo ---------------------------------------------------------------------------------*/
    GET_FROM_FIFO(RxCbPntr, ReturnRxIdPntr);
  }

  return ReturnRxIdPntr;
}


/*******************************************************************/
/*  PutUsedRxId                                                    */
/*  Put Rx ID into low or high priority fifo.                      */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*                                                                 */
/*******************************************************************/

void PutUsedRxId(NetType *NetPntr)
{
  RxCbType *RxCbPntr;

  /* determine fifo -------------------------------------------------------------------------------------------*/
  RxCbPntr = (NetPntr->RxDrCb.Priority == Low) ? &(NetPntr->RxCbLo) : &(NetPntr->RxCbHi);

  /* put RxIdPntr in fifo -------------------------------------------------------------------------------------*/
  PUT_IN_FIFO(RxCbPntr, (NetPntr->RxDrCb).RxIdPntr);
}


/*******************************************************************/
/*                                                                 */
/*  Get Tx ID from free-ID fifo and return ID pointer.             */
/*  On high priority check if ID-in-use fifo is empty.             */
/*  On low priority leave at least 1 ID in fifo.                   */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*                Priority  - high/low message priority            */
/*                                                                 */
/*                                                                 */
/*  Returnvalue:    == NULL - no ID available                      */
/*                  != NULL - pointer to Rx ID                     */
/*******************************************************************/

TxIdType  *GetFreeTxId(NetType *NetPntr)
{
  TxIdType *ReturnTxIdPntr;
  TxCbType *TxCbPntr = &(NetPntr->TxCbId);

  /* check if any TxId's are free -----------------------------------------------------------------------------*/
  if (NOT_EMPTY_FIFO(TxCbPntr)) {

    /* get TxIdPntr from fifo -----------------------------------------------------------------------------------*/
    GET_FROM_FIFO(TxCbPntr, ReturnTxIdPntr);

    return ReturnTxIdPntr;
  }
  return (TxIdType *)NULL;
}


/*******************************************************************/
/*                                                                 */
/*  Put Tx ID into fifo.                                           */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*               *TxIdPntr  - pointer to Tx ID                     */
/*                                                                 */
/*******************************************************************/

void  PutUsedTxId(TxCbType *TxCbPntr, TxIdType *TxIdPntr)
{
  /* put TxIdPntr in fifo -------------------------------------------------------------------------------------*/
  PUT_IN_FIFO(TxCbPntr, TxIdPntr);
}


/*******************************************************************/
/*  HelpPutFreeTxId                                                */
/*  Put Tx ID into free-ID-fifo.                                   */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*               *TxIdPntr  - pointer to Tx ID                     */
/*                                                                 */
/*******************************************************************/

void  PutFreeTxId(NetType *NetPntr, TxIdType *TxIdPntr)
{
  TxCbType *TxCbPntr = &(NetPntr->TxCbId);
  char      old_intlevel;

  INTS_OFF(&old_intlevel);


  /* put TxIdPntr in fifo -------------------------------------------------------------------------------------*/
  PUT_IN_FIFO(TxCbPntr, TxIdPntr);


  INTS_ON(&old_intlevel);
}


/*******************************************************************/
/*  HelpPutFreeTxId                                                */
/*  Put Tx ID into free-ID-fifo.                                   */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*               *TxIdPntr  - pointer to Tx ID                     */
/*                                                                 */
/*******************************************************************/

void  IntPutFreeTxId(NetType *NetPntr, TxIdType *TxIdPntr)
{
  TxCbType *TxCbPntr = &(NetPntr->TxCbId);
/*char      old_intlevel;  */

/*INTS_OFF(&old_intlevel);  */


  /* put TxIdPntr in fifo -------------------------------------------------------------------------------------*/
  PUT_IN_FIFO(TxCbPntr, TxIdPntr);


/*INTS_ON(&old_intlevel);  */
}


/*******************************************************************/
/*                                                                 */
/*  Find a Tx request.                                             */
/*                                                                 */
/*  Return pointer to Tx ID after a search in the Tx message pool  */
/*  in the following order:                                        */
/*    1 - high priority request in pool                            */
/*    2 - high priority request in fifo                            */
/*    3 - low priority request in pool                             */
/*    4 - low priority request in fifo                             */
/*                                                                 */
/*  TxPool[0] is the high priority pool                            */
/*  TxPool[1..4] are the low priority pools                        */
/*  TxPoolIx is the index into the actual low priority pool == 1   */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to struct Net                */
/*                                                                 */
/*  Returnvalue:    == NULL - no request available                 */
/*                  != NULL - pointer to ID                        */
/*******************************************************************/

TxIdType  *FindTxReq(NetType *NetPntr)
{
  TxPoolType   *HlpPoolPntr;
  TxIdType     *TxIdPntr;

  TxIdPntr = (TxIdType *)NULL;

  /* check if there are any messages --------------------------------------------------------------------------*/
  if (!((NetPntr->TxDrCb).TxRequest)) return((TxIdType *)NULL);

  /* check if there is a message in the current low priority pool ---------------------------------------------*/
  HlpPoolPntr = &((NetPntr->TxPool)[1] );
  if (HlpPoolPntr->Next != (TxIdType *)NULL) return HlpPoolPntr->Next;

  /* HlpPoolPntr->Next == NULL --------------------------------------------------------------------------------*/
  /* check if there is a message in the low priority fifo -----------------------------------------------------*/

  if (NOT_EMPTY_FIFO(&(NetPntr->TxCbLo))) {

    GET_FROM_FIFO(&(NetPntr->TxCbLo), TxIdPntr);
    TxIdPntr->Next = (TxIdType *)NULL;                   /* next-field new ID: NULL */

    /* there is not yet a pool for this node, chain TxId to empty pool ----------------------------------------*/
/*  HlpPoolPntr->Node = TxIdPntr->Destin;       WTH */
    HlpPoolPntr->Next = TxIdPntr;        /* next in chain pointer to ID */
    HlpPoolPntr->Last = TxIdPntr;        /* last in chain pointer to ID */
    HlpPoolPntr->RetryCount = RetryMax;  /* max number of retries       */
    TxIdPntr->TxIdPoolIx = 1;            /* LAB ch PoolIx               */

    return TxIdPntr;
  }

  TxIdPntr = (TxIdType *)NULL;       /* no Tx requests */
  (NetPntr->TxDrCb).TxRequest = FALSE;


  return (TxIdPntr);
}

/*******************************************************************/
/*                                                                 */
/*  Free a Tx request.                                             */
/*                                                                 */
/*  Release current ID from Tx pool. Return pointer to ID.         */
/*    1 - high priority release from TxPool[0]   does not exist    */
/*    2 - low priority release from TxPool[TxPoolIx]   == 1        */
/*                                                                 */
/*  TxPool[1..4] are the low priority pools with current index     */
/*  TxPoolIx is the index in the actual low priority pool          */
/*                                                                 */
/*  Parameter:   *NetPntr   - pointer to current struct Net        */
/*                                                                 */
/*  Returnvalue:  TxIdPntr  - pointer to released ID               */
/*                                                                 */
/*******************************************************************/

TxIdType  *FreeTxReq(NetType *NetPntr)

{
  TxPoolType  *HlpPoolPntr;
  TxIdType    *TxIdPntr;
  int         i,Ready;

  HlpPoolPntr = &(NetPntr->TxPool[1]);  /* error ? LAB */

  TxIdPntr = HlpPoolPntr->Next;                /* get pointer to ID to release */

  if (TxIdPntr == NetPntr->TxDrCb.TxIdPntr) {  /* LAB symptoombestrijding: if parameterpassing okay */
   if (TxIdPntr->Next == (TxIdType *)NULL) {               /* only one ID in chain? */
    HlpPoolPntr->Next = HlpPoolPntr->Last = (TxIdType *)NULL;   /* clear next/last field */
/*  HlpPoolPntr->Node = EmptyPool;    */         /* mark pool item empty */
   } else {
    HlpPoolPntr->Next = TxIdPntr->Next;        /* release old ID from chain */
 /* TxIdPntr->Next = NULL;
    HlpPoolPntr->Node = TxIdPntr->Destin; */   /* copy new destination address */
    HlpPoolPntr->RetryCount = RetryMax;          /* reload number of retries */
   }
  } else {                                       /* LAB else  symptoombestrijding errorsituation */
    for ( i = 0,Ready = 0 ; i < TxPoolMax && Ready == 0 ; ) {  /* scan TxPoolOrganisation  */
      if (NetPntr->TxPool[i].Next == NetPntr->TxDrCb.TxIdPntr) {       /* if (this Id in this pool */
        Ready = 1;
      } else {
        i+=1;
      }
    }
    if (Ready == 1) {
/*    LAB_TestError3++;                      */
/*    LAB_TestError4 = (int) HlpPoolPntr;    */
      HlpPoolPntr = &(NetPntr->TxPool[i]);           /* set helpvar */
      TxIdPntr  = HlpPoolPntr->Next;               /* get pointer to ID to release */
      if (TxIdPntr->Next == (TxIdType *)NULL) {                /* only one ID in chain? */
        HlpPoolPntr->Next = HlpPoolPntr->Last = (TxIdType *)NULL;/* clear next/last field */
        HlpPoolPntr->Node = EmptyPool;               /* mark pool item empty */
      } else {
        HlpPoolPntr->Next = TxIdPntr->Next;        /* release old ID from chain */
        TxIdPntr->Next = (TxIdType *)NULL;
/*      HlpPoolPntr->Node = TxIdPntr->Destin; WTH */  /* copy new destination address */
        HlpPoolPntr->RetryCount = RetryMax;          /* reload number of retries */
      }
    } else {                                         /* Id not in pool */
/*    LAB_TestError0++;                         */
/*    LAB_TestError1 = (int) HlpPoolPntr;       */
      *((int *)0x44)= 9;                               /* LAB break */
      HlpPoolPntr->Next = HlpPoolPntr->Last = (TxIdType *)NULL;  /* clear next/last field */
      HlpPoolPntr->Node = EmptyPool;                 /* mark pool item empty */
/*    LAB_TestError0++;         */
      HlpPoolPntr->Next = HlpPoolPntr->Last = (TxIdType *)NULL;  /*clear next/last field */
      HlpPoolPntr->Node = EmptyPool;                 /* mark pool item empty */
    }
  }                                                  /* LAB einde symptoombestrijding */
  return(TxIdPntr);
}

