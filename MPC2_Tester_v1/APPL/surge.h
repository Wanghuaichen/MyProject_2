#ifndef SURGE_INCLUDED
#define SURGE_INCLUDED  1


 
#define MILK_METER_ADDRESS 1
#define NR_OFF_MILKMETERS  1  
#define VALVE_TIME  150  //1,5 sec 
              
#define SERIAL_CODE     0xd9
#define POLL_CODE       0xc0
#define DETACH_CODE     0xc7
#define CLEAR_CODE      0x47
#define COW_ALARM_CODE  0xe0

#define METER_WITH_DATA     0x87
#define METER_WITH_NO_DATA  0x80
 
#define DHIA_OFF        0x0
#define DHIA_ON         0x1

#define ALARM_ON        0x41
#define ALARM_OFF       0x42
#define ALARM_FLASH     0x44 
#define LOW_PRODUCTION  0x41
 
#define RESULT_CODE_SHORT 2
#define RESULT_CODE_LONG  9  

#define MAX_RETRIES_SEND  50
#define MAX_RETRIES_READ  50

typedef struct {
  unsigned char    RelAddress;
  unsigned short   SerieNumber;
  unsigned short   DelayTime;
  unsigned char    DHIAStatus;
  unsigned char    MilkmeterCode;
  unsigned char    ResetFlag;
  unsigned char    AlarmStatus;
  unsigned short   CowNumber;
  unsigned short   PrevCowNumber;
  unsigned short   MilkYield;
  unsigned short   CumMilkYield;
  unsigned char    MilkmeterStatus;
  } SURGE_MILKMETER_INFO;

               
typedef struct {
  unsigned char  SerialCode;   //Verify serial number code
  unsigned char  RelAddress;   //Relative Address
  unsigned char  NA;           //N/A
  unsigned short SerialNumber; //Serial Number
} SERIAL_NUMBER;                               
  
typedef struct {
  unsigned char  PollCode;     //Pollcode
  unsigned char  RelAddress;   //Relative Address
} POLL_METER;                               
  
typedef struct {
  unsigned char  ResultCode;   //Result code
  unsigned char  RelAddress;   //Relative Address
  unsigned short MilkYield;    //MilkYield
  unsigned short NA;           //N/A
  unsigned short CowNumber;    //CowNumber 
  unsigned char  ResetFlag;    //ResetFlag 
} RESULT_METER;         
 
typedef struct {
  unsigned char  DetachCode;   //Detach/Delay code
  unsigned char  NA;           //N/A
  unsigned char  DhiaMode;     //Dhia mode select
  unsigned short DetachDelay;  //Detach Delay
} DETACH_DHIA;         
             
typedef struct {
  unsigned char  ClearMilk;    //Clear Meter Production code
  unsigned char  RelAddress;   //Relative Address
} CLEAR_METER;

typedef struct {
  unsigned char  SendData;     //Send Data code
  unsigned char  RelAddress;   //Relative Adress
  unsigned short CowNumber;    //CowNumber
  unsigned char AlarmCode;     //Alarm Status Code
  } COW_ALARM;         
                               
                      
 

extern void SurgeMeasuring(void);
extern void HndlSurgeTakeOffInt(void);
extern void HndlSurgeMsg(void);


#endif
