#ifndef FLOW_RECEIVE_INCLUDED
#define FLOW_RECEIVE_INCLUDED   1



#define FLOW_SCAN_TIME          40000

extern unsigned char StopNetReceive;    
     
extern void HandleMess(void);  
non_banked void HandleMess_Non_Banked(void); 
#endif
