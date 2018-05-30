#ifndef SCHEDULE_INCLUDED
#define SCHEDULE_INCLUDED 1




/* functie declaraties ------------------------------------------------------------------------*/
non_banked void Scheduler_Start(void);
non_banked void Scheduler(void);
non_banked void TaskExit(void);
non_banked void StopScheduler(void);
non_banked void ContScheduler(void);
non_banked void TaskDelay(unsigned char);
non_banked void TaskStop(unsigned char);
non_banked void TaskStart(unsigned char);
non_banked void Scheduler_Entry(void);




#endif
