/**
  @file
  @author Stefan Frings
*/

#include "global.h"
#include "obulive.h"

TemplateCache* templateCache;
HttpSessionStore* sessionStore;
StaticFileController* staticFileController;
FileLogger* logger;

RsuSdk * sdk_g ;
MyThread * obu_live_g;

//十六进制字符串转二进制数据
const unsigned char bin_table[256] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 1-8
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 9-16
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 17-24
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 25-32
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 33-40
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 41-48
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,// 49-56
    0x08,0x09,0x00,0x00,0x00,0x00,0x00,0x00,// 57-64
    0x00,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,// 65-72
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 73-80
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 81-88
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 89-96
    0x00,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,// 97-104
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 105-112
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 112-120
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 121-128
    //<-128, 这里开始就是非法的字符了，当做0处理
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// row17--->
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 32
    //<- 256
};

const unsigned char hex_table[17] = "0123456789ABCDEF";

RsuSdk::RsuSdk()
{
     Init();
}

void RsuSdk::Init()
{

    m_pDevLib = nullptr;

    RSU_Open = nullptr;
    RSU_Close = nullptr;
    RSU_INIT_rq = nullptr;
    RSU_INIT_rs = nullptr;
    INITIALISATION_rq = nullptr;
    INITIALISATION_rs = nullptr;
    TransferChannel_rq = nullptr;
    TransferChannel_rs = nullptr;
    SetMMI_rq = nullptr;
    SetMMI_rs = nullptr;
    EVENT_REPORT_rq = nullptr;

    m_pOBUKeepOnPowerTimer = nullptr;
    m_pOBUKeepOnPowerTimer = new QTimer(this);
    m_pOBUKeepOnPowerTimer->setInterval(OBU_ON_POWER_INTERVAL);
    connect(m_pOBUKeepOnPowerTimer,SIGNAL(timeout()),this,SLOT(testing()));

    connect(this, SIGNAL(start_testing_signal()), this, SLOT(start_test_timer()));
    connect(this, SIGNAL(stop_testing_signal()),  this, SLOT(stop_test_timer()));

    m_lfd = 0;


    DoLoadDLL();


}
QString RsuSdk::GetFunLineInfor(const char* sFuncName,int iLineNum)
{
    return QString().sprintf("%-24s:%.05d:",sFuncName,iLineNum);
}


void RsuSdk::DoLoadDLL()
{
    if((nullptr != m_pDevLib) && (m_pDevLib->isLoaded()))
    {
       qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"已加载设备动态库";
        return;
    }

    if(nullptr == m_pDevLib)
    {
        m_pDevLib = new QLibrary("GvApi_GBTC.dll");
    }

    if(!m_pDevLib->load())
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"加载设备动态库失败"<<m_pDevLib->errorString();
        return;
    }

    RSU_Open = reinterpret_cast<QRSU_Open>(m_pDevLib->resolve("RSU_Open"));
    if(!RSU_Open)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取RSU_Open函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    RSU_Close = reinterpret_cast<QRSU_Close>(m_pDevLib->resolve("RSU_Close"));
    if(!RSU_Close)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取RSU_Close函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    RSU_INIT_rq = reinterpret_cast<QRSU_INIT_rq>(m_pDevLib->resolve("RSU_INIT_rq"));
    if(!RSU_INIT_rq)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取RSU_INIT_rq函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    RSU_INIT_rs = reinterpret_cast<QRSU_INIT_rs>(m_pDevLib->resolve("RSU_INIT_rs"));
    if(!RSU_INIT_rs)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取RSU_INIT_rs函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    INITIALISATION_rq = reinterpret_cast<QINITIALISATION_rq>(m_pDevLib->resolve("INITIALISATION_rq"));
    if(!INITIALISATION_rq)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取INITIALISATION_rq函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    INITIALISATION_rs = reinterpret_cast<QINITIALISATION_rs>(m_pDevLib->resolve("INITIALISATION_rs"));
    if(!INITIALISATION_rq)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取INITIALISATION_rs函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    TransferChannel_rq = reinterpret_cast<QTransferChannel_rq>(m_pDevLib->resolve("TransferChannel_rq"));
    if(!TransferChannel_rq)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取TransferChannel_rq函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    TransferChannel_rs = reinterpret_cast<QTransferChannel_rs>(m_pDevLib->resolve("TransferChannel_rs"));
    if(!TransferChannel_rs)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取TransferChannel_rs函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    SetMMI_rq = reinterpret_cast<QSetMMI_rq>(m_pDevLib->resolve("SetMMI_rq"));
    if(!SetMMI_rq)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取SetMMI_rq函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    SetMMI_rs = reinterpret_cast<QSetMMI_rs>(m_pDevLib->resolve("SetMMI_rs"));
    if(!SetMMI_rs)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取SetMMI_rs函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    EVENT_REPORT_rq = reinterpret_cast<QEVENT_REPORT_rq>(m_pDevLib->resolve("EVENT_REPORT_rq"));
    if(!EVENT_REPORT_rq)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"获取EVENT_REPORT_rq函数失败"<<m_pDevLib->errorString();
        m_pDevLib->unload();
        return;
    }

    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"已加载设备动态库";
}

void  RsuSdk::DoDevice()
{
    if((nullptr == m_pDevLib) || !m_pDevLib->isLoaded())
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备动态库还未加载";
        return;
    }

    char sDevInfor[32] = {0};

    memcpy(sDevInfor,"COM7",4);

    if(m_lfd > 0)
    {
        RSU_Close(m_lfd);
        m_lfd = 0;
        //ui->pbDevice->setText("连接设备");
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备已断开";
    }
    else
    {
        m_lfd = RSU_Open(0,sDevInfor,0);
        if(m_lfd <= 0)
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"连接设备失败";
            return;
        }

        int iRet = RSU_INIT_rq(m_lfd,nullptr,0,31,0,DEV_TIMEOUT);
        if(0 != iRet)
        {
            RSU_Close(m_lfd);
            m_lfd = 0;
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备初始化请求失败";
            return;
        }

        int iStatus = 0, iRlen = 0;
        char szRsuInfo[12] = {0};
        iRet = RSU_INIT_rs(m_lfd,&iStatus,&iRlen,szRsuInfo, DEV_TIMEOUT);
        if(0 != iRet)
        {
            RSU_Close(m_lfd);
            m_lfd = 0;
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备初始化响应失败";
            return;
        }

        //ui->pbDevice->setText("断开设备");
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备已连接";
    }
}

int RsuSdk::OpenRsu(QString devname)
{
    if((nullptr == m_pDevLib) || !m_pDevLib->isLoaded())
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备动态库还未加载";
        return -1;
    }
    // if the device has been opened, just close it and reopen.
    if(m_lfd > 0)
    {
        RSU_Close(m_lfd);
        m_lfd = 0;
        //ui->pbDevice->setText("连接设备");
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备已断开";
    }

    m_lfd = RSU_Open(0, devname.toLatin1().data(), 0);
    if(m_lfd <= 0)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"连接设备失败";
        return -1;
    }

    return  0;

}

int RsuSdk::CloseRsu()
{
    if((nullptr == m_pDevLib) || !m_pDevLib->isLoaded())
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备动态库还未加载";
        return -1;
    }

    RSU_Close(m_lfd);
    m_lfd = 0;
        //ui->pbDevice->setText("连接设备");
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备已断开";
    return 0;


}

int RsuSdk::InitRsu(char *szTime, int iBstInterval, int iPower, int iChannelID, int iTimerOut, char* rsuInfo, int lenRsuInfo)
{
    if(m_lfd <= 0)
    {

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备未打开";
        return -1;

    }
    else
    {
        int iRet = RSU_INIT_rq(m_lfd,nullptr, iBstInterval, iPower, iChannelID, DEV_TIMEOUT);
        if(0 != iRet)
        {
            RSU_Close(m_lfd);
            m_lfd = 0;
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备初始化请求失败";
            return -1;
        }

        int iStatus = 0, iRlen = 0;
        char szRsuInfo[128] = {0};
        iRet = RSU_INIT_rs(m_lfd,&iStatus,&iRlen,szRsuInfo, DEV_TIMEOUT);
        if(0 != iRet)
        {
            RSU_Close(m_lfd);
            m_lfd = 0;
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备初始化响应失败";
            return -1;
        }

        //unsigned char hexRsuInfo[256]  = {0};
        BinToHex((unsigned char*)szRsuInfo, (unsigned char*)rsuInfo, iRlen);

        QString retinfo = (char*)rsuInfo;

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"len:"<< iRlen * 2 <<"rsu return info:" << rsuInfo;
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"rsu return info:" << retinfo;
        lenRsuInfo = iRlen * 2;
        rsuInfo[lenRsuInfo] = '\0';

        //ui->pbDevice->setText("断开设备");
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"设备已连接";
    }

    return 0;

}

int RsuSdk::TransferChannel(int *iMode, int *iDID, int * iChannelID, int *iAPDUList, char *szAPDU, char *retAPDU, int *iReturnStatus)
{
    KeepOBUOnPowerStop();
    QThread::msleep(50);  //make sure the keep alive timer has been stoped and disabled.
    unsigned char szAPDUin[128] = {0},CosRsp[128] = {0};

    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"start to send transchannel";

    HexToBin((unsigned char*)szAPDU, szAPDUin, strlen(szAPDU) );
    int iRet = TransferChannel_rq(m_lfd, *iMode, *iDID, *iChannelID, *iAPDUList, reinterpret_cast<char*>(szAPDUin), DEV_TIMEOUT);

    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"transchannel ret:"<< iRet;

    if(0 != iRet) return iRet;


    *iAPDUList = 0;
    memset(szAPDUin,0x00,sizeof(szAPDUin));
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"start to recv transchannel";

    iRet = TransferChannel_rs(m_lfd, iDID, iChannelID, iAPDUList, (char*)szAPDUin, iReturnStatus, DEV_TIMEOUT);
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"recv transchannel ret:" << iRet;

    if(0 != iRet) return iRet;

    int len = GetLenOfApdu(szAPDUin, *iAPDUList);

    BinToHex(szAPDUin,reinterpret_cast<unsigned char*>(retAPDU), len);


    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"len:" << len;

    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"transfer channel response:" << QString(retAPDU);


    KeepOBUOnPowerStart();
    return 0;

}

int RsuSdk::HexToBin(unsigned char *shex, unsigned char *sbin, int shex_len)
{
    int len = 0, j = 0;
    unsigned char ch = 0;// , val = 0;
    len = (shex_len / 2) * 2;
    if (!shex || shex[0] == 0 || len == 0)
    {
        return 0;
    }
    for (int i = 0; i < (len); i++)
    {
        //查表法，速度飞快！
        if (i & 1)
        {
            ch = static_cast<unsigned char>((sbin[j] << 4) | (bin_table[shex[i]])); //C++ 11用staitc_cast 代替 强转
            sbin[j++] = ch;
        }
        else
        {
            sbin[j] = bin_table[shex[i]];
        }
    }//for
    //sbin[j]=0;//不改变最后一位的值！
    return j;
}

int RsuSdk::BinToHex(unsigned char *sbin, unsigned char *shex, int sbin_len)
{
    int j = 0;
    for (int i = 0; i < sbin_len; i++)
    {
        unsigned char ch = sbin[i];
        shex[j++] = hex_table[ch >> 4];
        shex[j++] = hex_table[ch & 0x0F];
    }
    shex[j] = '\0'; //最后加一个结束符。
    return j;
}

int RsuSdk::GetSw12(unsigned char *szSW12)
{
    int ret = 0x8888;
    if (szSW12)
    {
        ret = szSW12[0] * 256 + szSW12[1];
    }
    return ret == 0 ? 0x8888 : ret;
}

int RsuSdk::GetOneApdu(unsigned char *szApdu, int iApdulist, int iIndex, unsigned char *szOutApdu, int *iOutApdLen)
{
    if (iOutApdLen) { *iOutApdLen = 0; }
    if (szOutApdu) { memset(szOutApdu, 0, 128); }
    int iSW12 = 0x8888;
    //index start at 0
    if (iIndex >= iApdulist)
    {
        return -9007;
    }
    //04 xx xx 90 00 05 xx xx xx 90 00
    int len = 0;
    for (int i = 0; i < iIndex; i++)
    {
        len += szApdu[len];
        len += 1;
    }
    uchar *p = szApdu + len; // 04  xx xx 90 00
    len = *p++; // 04
    if (len < 2)
    {
        return -9004;//长度至少2字节
    }
    if (szOutApdu)
    {
        memcpy(szOutApdu, p, std::uint32_t(len - 2));
    }

    if (iOutApdLen)
    {
        *iOutApdLen = len - 2;
    }

    iSW12 = GetSw12(&p[len - 2]);

    return iSW12;
}

int RsuSdk::GetLenOfApdu(unsigned char *szApdu, int iApdulist)
{
    //04 xx xx 90 00 05 xx xx xx 90 00
    int len = 0;
    for (int i = 0; i < iApdulist; i++)
    {
        len += szApdu[len];
        len += 1;
    }
    return len;
}

int RsuSdk::KeepOBUOnPowerStart()
{
    //if(!m_pOBUKeepOnPowerTimer->isActive())
    //{
       // m_pOBUKeepOnPowerTimer->start();
   // }

    MyThread::enable_flag = true;
    //qWarning()<< "send start_keep_live_timer";
    //emit start_keep_live_timer();
    emit start_testing_signal();
    return 0;
}

int RsuSdk::KeepOBUOnPowerStop()
{
    //if(m_pOBUKeepOnPowerTimer->isActive())
    //{
        //m_pOBUKeepOnPowerTimer->stop();
    //}
    MyThread::enable_flag = false;
    emit stop_testing_signal();
    //emit stop_keep_live_timer();

    return 0;
}

int RsuSdk::OBUSearch(char* sObuMac)
{
    bool bFound = false;
    int iRet = -1,iCnt = 0,iRlen = 0;
    int iReturnStatus = 0,iProfile = 0,iApplicationList = 0;
    unsigned char Application[128] = {0},ObuConfiguration[16] = {0},sTemp[256] = {0};

    for(iCnt = 0; iCnt < OBU_FIND_TIMES; iCnt++)
    {
        memcpy(Application,"\x04\x20\x1B\x00\x1B",5); //预读系统信息和0015文件
        iRet = INITIALISATION_rq(m_lfd, nullptr, nullptr, 0, 0, reinterpret_cast<char*>(Application), 0, DEV_TIMEOUT);
        if(iRet == 0)
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"INITIALISATION_rq 成功";
        }
        else
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"INITIALISATION_rq 失败, 错误代码:"<<iRet;
            continue;
        }

        memset(Application,0,sizeof(Application));
        iRet = INITIALISATION_rs(m_lfd, &iReturnStatus, &iProfile, &iApplicationList, reinterpret_cast<char*>(Application), reinterpret_cast<char*>(ObuConfiguration), DEV_TIMEOUT);
        if(iRet == 0)
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"INITIALISATION_rs 成功";
        }
        else
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"INITIALISATION_rs 失败, 错误代码:"<<iRet;
        }

        if(iRet==0)
        {
            iRlen = 1 ;
            iCnt = Application[0];
            for(int i=0;i<iCnt;i++)
            {
                iRlen += Application[iRlen];
                iRlen +=1;
            }
            BinToHex(Application,sTemp,iRlen);
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Application:"<<reinterpret_cast<char*>(sTemp);

            memset(sTemp,'\0',sizeof(sTemp));
            BinToHex(ObuConfiguration,sTemp,7);
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"已搜索到OBU,ObuConfiguration:"<<reinterpret_cast<char*>(sTemp);

            BinToHex(ObuConfiguration,reinterpret_cast<unsigned char*>(sObuMac),4);
        }

        if(iRet != 0)
        {
            continue;
        }
        else
        {
            bFound = true;
            break;
        }
    }

    if(!bFound)
    {
        return -1;
    }

    KeepOBUOnPowerStart();

    return 0;
}

int RsuSdk::OBURelease(int iSetMMI,int iEventReport)
{
    int iDID = 1,iReturnStatus = 0;

    KeepOBUOnPowerStop();

    if(iSetMMI)
    {
        SetMMI_rq(m_lfd, 1, iDID, 0, DEV_TIMEOUT);
        SetMMI_rs(m_lfd, &iDID, &iReturnStatus, DEV_TIMEOUT);
    }

    if(iEventReport)
    {
        EVENT_REPORT_rq(m_lfd, 0, iDID, 1, DEV_TIMEOUT);
    }

    return 0;
}

int RsuSdk::OBUICSelectFile(int iChannel,unsigned short usFileID)
{
    int iMode = 1,iDID = 1,iChannelID = -1,iAPDUList = 0,iReturnStatus = 0,CosRspLen = 0;
    unsigned char szAPDU[128] = {0},CosRsp[128] = {0};

    KeepOBUOnPowerStop();

    iChannelID = iChannel;
    iAPDUList = 1;
    memcpy(szAPDU,"\x07\x00\xA4\x00\x00\x02",6);
    szAPDU[6] = static_cast<unsigned char>(usFileID/256);
    szAPDU[7] = static_cast<unsigned char>(usFileID%256);
    int iRet = TransferChannel_rq(m_lfd, iMode, iDID, iChannelID, iAPDUList, reinterpret_cast<char*>(szAPDU), DEV_TIMEOUT);
    if(0 != iRet) return iRet;
    iAPDUList = 0;
    memset(szAPDU,0x00,sizeof(szAPDU));
    iRet = TransferChannel_rs(m_lfd, &iDID, &iChannelID, &iAPDUList, reinterpret_cast<char*>(szAPDU), &iReturnStatus, DEV_TIMEOUT);
    if(0 != iRet) return iRet;

    iRet = GetOneApdu(szAPDU,iAPDUList,0,CosRsp,&CosRspLen);
    if(0x9000 != iRet) return iRet;

    KeepOBUOnPowerStart();

    return 0;
}


void RsuSdk::DoObuKeepOnPowerTimeOutHandle()
{
    int iMode = 1,iDID = 1,iChannelID = ESAM_CHANNEL,iAPDUList = 0,iReturnStatus = 0;
    char szAPDU[128] = {0};

    TransferChannel_rq(m_lfd, iMode, iDID, iChannelID, iAPDUList, szAPDU, DEV_TIMEOUT);
    TransferChannel_rs(m_lfd, &iDID, &iChannelID, &iAPDUList, szAPDU, &iReturnStatus, DEV_TIMEOUT);
}


void RsuSdk::testing()
{
    qWarning() << "period trigger testing timer";
    DoObuKeepOnPowerTimeOutHandle();
}



void RsuSdk::start_test_timer()
{
    qWarning() << "start testing timer";
    if(!m_pOBUKeepOnPowerTimer->isActive())
    {
        m_pOBUKeepOnPowerTimer->start();
    }
}

void RsuSdk::stop_test_timer()
{
    qWarning() << "stop testing timer";
    if(m_pOBUKeepOnPowerTimer->isActive())
    {
        m_pOBUKeepOnPowerTimer->stop();
    }
}


int RsuSdk::start_testing()
{
    //if(m_pOBUKeepOnPowerTimer->isActive())
    //{
        //m_pOBUKeepOnPowerTimer->stop();
    //}
    //MyThread::enable_flag = false;

    //emit start_testing_signal();

    return 0;
}
