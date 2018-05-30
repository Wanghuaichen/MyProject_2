#ifndef ROM_STARTNET_INCLUDED
#define ROM_STARTNET_INCLUDED 1


#define         ArbModeVCP      0


#define CHANNEL 1
//#define BUFFERS 2
#define BUFSIZE 255

typedef struct {
  void              *msglnk;            /* USX link to next message in mailbox */
  unsigned char     msgpri;             /* USX priority for message in mailbox */
  unsigned char     channel;            /* NETWORK channel of sender */
  unsigned char     address_src;        /* NETWORK address of sender of the message */
  unsigned char     address_dest;       /* NETWORK address of receiver*/
  unsigned char     message_length;     /* length of message */
  unsigned short    message_id;         /* message identifier */
  char              message_start;      /* first byte of the message */
  } NETIO_MAIL;


/* function prototypes -----------------------------------------------*/

void RomInitStartNet(void);
void RomInitNetw(void);


extern signed short RomPeripheralAddress;

#endif
