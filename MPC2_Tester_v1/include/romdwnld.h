#ifndef ROM_DWN_LD_INCLUDED
#define ROM_DWN_LD_INCLUDED 1



#define         DOWN_LOAD_ERASING               1
#define         DOWN_LOAD_ERASING_OK            2
#define         DOWN_LOAD_ERASING_FAIL          3
#define         DOWN_LOAD_PROGRAMMING           4
#define         DOWN_LOAD_PROGRAMMING_OK        5
#define         DOWN_LOAD_PROGRAMMING_FAIL      6
#define         DOWN_LOAD_PROG                  7
/*efine         DOWN_LOAD_DBMS                  8               */
#define         DOWN_LOAD_PROG_COMPLETE         9
/*efine         DOWN_LOAD_DBMS_COMPLETE         10              */
#define         DOWN_LOAD_PROG_FIRST_BYTE       11
#define         DOWN_LOAD_FIRST_BYTE            12
#define         DOWN_LOAD_PROG_FAIL             13
/*efine         DOWN_LOAD_DBMS_FAIL             14              */
#define         DOWN_LOAD_WAITING               15
#define         DOWN_LOAD_COMPLETE              16
#define         DOWN_LOAD_VERSION_FAIL          17
#define         DOWN_LOAD_PROG_COMPLETE_FAIL    18
#define         DOWN_LOAD_START                 1234

#define         ERROR_ERASING_CLEARING          1
#define         ERROR_ERASING_ERASING           2
#define         ERROR_PROGRAMMING               255





enum dbase_commands {
  ddb_PRESENT = 1,
  NO_ddb_PRESENT,
  PROG_PRESENT,
  NO_PROG_PRESENT
};


enum {
  OFF = 0,
  ON
};


enum download_result_type {
  DLR_OK,
  DLR_WAITING,
  DLR_ERROR,
  DLR_ERROR_VERSION
  };



extern void Reset(void);
extern void DownLoading(void);
extern void DownLoadPROGProgramming(NETIO_MAIL *MailPtr);

extern unsigned short DownLoadStatus;

#endif
