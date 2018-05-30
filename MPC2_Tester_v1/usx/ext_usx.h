/********************************************************************************/
/*  ext_usx.h                                                                   */
/*                                                                              */
/*  external declarations of USX-functions for 68HC11                           */
/*                                                                              */
/********************************************************************************/
/*  author      : L.A. Boulogne                 (c) NEDAP GROENLO               */
/*  date        : 03-02-1992                                                    */
/********************************************************************************/

#ifndef EXT_USX_INCLUDED
#define EXT_USX_INCLUDED  1

  extern short UsxRunTsk ( void (*TaskName)(void),
                         short StartPriorTsk,
                         short StackSize,
                         short NrOfArguments,
                         ...                    );
  extern short UsxKllTsk (short SlotNrTsk);
  extern short UsxPriTsk (short SlotNrTsk, short PriorTsk);
  extern short UsxChkPri (short SlotNrTsk);
  extern short UsxSltTsk ( void (*TaskName)(void) );
  extern short UsxScdTsk ( void );
  extern short UsxDlyTsk (short SlotNrTsk,
                        short DelayType,
                        short DelayCount);
  extern short UsxWkeTsk (short SlotNrTsk);
  extern short UsxSetClk (short Hour, short Minute, short Second);
  extern short UsxGetClk (short *Hour, short *Minute, short *Second);
  extern short UsxSetVct( short VC_time, short Second);
  extern short UsxSetVcd( short VC_date);
  extern short UsxGetVct( void );
  extern unsigned long  UsxGetAbsTime( void );

  extern short UsxGetRes (short ResNr);
  extern short UsxReqRes (short ResNr);
  extern short UsxRelRes (short ResNr);
  extern short UsxChkRes (short ResNr);

  extern short UsxSetEvt (short EvtNr);
  extern short UsxClrEvt (short EvtNr);
  extern short UsxChkEvt (short EvtNr);
  extern short UsxWteSet (short EvtNr);
  extern short UsxWteClr (short EvtNr);

  extern void *UsxGetMsg (short MailboxNr);
  extern void *UsxReqMsg (short MailboxNr);
  extern short UsxSndMsg (short   MailboxNr, void *MsgAddress);
  extern void *UsxChkMsg (short MailboxNr);

  extern short UsxReqMem (short NrBytesRequested, short TypeRequested );
  extern short UsxRelMem (void *AddressMem);
  extern short UsxChkMem (short *MemBlocksFree);

  extern short UsxDbmsPtrBlock (short NrOfFiles);
  extern void **UsxDbmsPtr (short FileNr);

  extern void  UsxInitStackTable (void);
  extern short UsxCheckStack (short TskSlotNr);


  extern short COP_reset_counter;
  extern unsigned short Counter_10ms;

typedef short MAILBOX_NUMBER;

typedef enum {
  PROCESS_DEPENDENT = 0,      /* lifetime depends on task */
  PROCESS_INDEPENDENT = 1     /* lifetime independent of task */
  } UsxMemBlockType;

#endif
