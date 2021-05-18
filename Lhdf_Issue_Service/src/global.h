/**
  @file
  @author Stefan Frings
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QLibrary>

#include "templatecache.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "filelogger.h"

using namespace stefanfrings;

/**
  Global objects that are shared by multiple source files
  of this project.
*/

/** Cache for template files */
extern TemplateCache* templateCache;

/** Storage for session cookies */
extern  HttpSessionStore* sessionStore;

/** Controller for static files */
extern  StaticFileController* staticFileController;

/** Redirects log messages to a file */
extern  FileLogger* logger;

//forward declaration.
class RsuSdk;

extern RsuSdk * sdk_g;
class MyThread;

extern MyThread * obu_live_g;


/** M600T device SDK interface */
typedef long (__stdcall *QRSU_Open)(
        int   iMode,
        char* szAddress,
        int   iPort
        );

typedef int  (__stdcall *QRSU_Close)(
        long fd
        );

typedef int  (__stdcall *QRSU_INIT_rq)(
        long  fd,
        char* szTime,
        int   iBstInterval,
        int   iPower,
        int   iChannelID,
        int   iTimeOut
        );

typedef int  (__stdcall *QRSU_INIT_rs)(
        long  fd,
        int*  iStatus,
        int*  iRlen,
        char* szRsuInfo,
        int   iTimeOut
        );

typedef int  (__stdcall *QINITIALISATION_rq)(
        long  fd,
        char* szBeaconID,
        char* szTime,
        int   iProfile,
        int   iMandApplicationlist,
        char* szMandApplication,
        int   iProfilelist,
        int   iTimeOut
        );

typedef int  (__stdcall *QINITIALISATION_rs)(
        long  fd,
        int*  iStatus,
        int*  iProfile,
        int*  iApplicationlist,
        char* szApplication,
        char* szObuCfg,
        int   iTimeOut
        );

typedef int  (__stdcall *QTransferChannel_rq)(
        long  fd,
        int   iMode,
        int   iDID,
        int   iChannelID,
        int   iAPDUList,
        char* szAPDU,
        int   iTimeOut
        );

typedef int  (__stdcall *QTransferChannel_rs)(
        long  fd,
        int*  iDID,
        int*  iChannelID,
        int*  iAPDUList,
        char* szAPDU,
        int*  iReturnStatus,
        int   iTimeOut
        );

typedef int  (__stdcall *QSetMMI_rq)(
        long fd,
        int  iMode,
        int  iDID,
        int  iSetMMIPara,
        int  iTimeOut
        );

typedef int  (__stdcall *QSetMMI_rs)(
        long fd,
        int* iDID,
        int* iReturnStatus,
        int  iTimeOut
        );

typedef int  (__stdcall *QEVENT_REPORT_rq)(
        long fd,
        int  iMode,
        int  iDID,
        int  iEventType,
        int  iTimeOut
        );

#define OBU_ON_POWER_INTERVAL   200
#define DEV_TIMEOUT             5000
#define IC_CHANNEL              1
#define ESAM_CHANNEL            2
#define OBU_FIND_TIMES          6


class RsuSdk :public QObject
{
    Q_OBJECT
public:
    RsuSdk();
    virtual ~RsuSdk(){;};
    void Init();

    QRSU_Open                   RSU_Open;
    QRSU_Close                  RSU_Close;
    QRSU_INIT_rq                RSU_INIT_rq;
    QRSU_INIT_rs                RSU_INIT_rs;
    QINITIALISATION_rq          INITIALISATION_rq;
    QINITIALISATION_rs          INITIALISATION_rs;
    QTransferChannel_rq         TransferChannel_rq;
    QTransferChannel_rs         TransferChannel_rs;
    QSetMMI_rq                  SetMMI_rq;
    QSetMMI_rs                  SetMMI_rs;
    QEVENT_REPORT_rq            EVENT_REPORT_rq;



    int  BinToHex(unsigned char *sbin, unsigned char *shex, int sbin_len);
    int  HexToBin(unsigned char *shex, unsigned char *sbin, int shex_len);
    int  GetSw12(unsigned char *szSW12);
    int  GetOneApdu(unsigned char *szApdu, int iApdulist, int iIndex, unsigned char *szOutApdu, int *iOutApdLen);

    int  GetLenOfApdu(unsigned char *szApdu, int iApdulist);


    int  KeepOBUOnPowerStart();
    int  KeepOBUOnPowerStop();

    int  start_testing();

    int  OBUSearch(char* sObuMac);
    int  OBURelease(int iSetMMI,int iEventReport);
    int  OBUICSelectFile(int iChannel,unsigned short usFileID);


public:
    QLibrary        *m_pDevLib;
    long             m_lfd;
    QTimer          *m_pOBUKeepOnPowerTimer;


    void DoLoadDLL();



public:
    QString GetFunLineInfor(const char* sFuncName,int iLineNum);
    void DoDevice();
    int  OpenRsu(QString devname);
    int  CloseRsu();
    // rsuInfo : return parameter. returned rsu init response info.
    // szRsuInfo  in/out parameter.
    int  InitRsu(char* szTime, int iBstInterval, int iPower, int iChannelID, int iTimerOut, char* rsuInfo, int lenRsuInfo);


    //in/out parameter. iMode, iMode, iChannelID, iAPDUList.
    int  TransferChannel(int* iMode, int* iDID, int* iChannelID, int* iAPDUList, char* szAPDU, char* retAPDU, int* iReturnStatus);

    bool is_device_open()
    {
        return m_lfd > 0;
    }

signals:
    void start_keep_live_timer();
    void stop_keep_live_timer();
    void start_testing_signal();
    void stop_testing_signal();


public slots:
    void DoObuKeepOnPowerTimeOutHandle();
    void stop_test_timer();
    void start_test_timer();
    void testing();
};





#endif // GLOBAL_H
