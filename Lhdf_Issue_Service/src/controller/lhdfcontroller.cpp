#include "lhdfcontroller.h"
#include "./src/global.h"

#include <QVariant>
#include <QDateTime>
#include <QThread>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

extern QString GetFunLineInfor(const char* sFuncName,int iLineNum);

QMap<int,QString> LhdfController::status_map;

LhdfController::LhdfController()
{
    status_map[200] = "OK";
    status_map[400] = "Bad Request [Invalid or Incomplete Parameters]";
    status_map[401] = "Not Found";
    status_map[500] = "Internal Server Error";

}


void LhdfController::service(HttpRequest& request, HttpResponse& response)
{

    response.setHeader("Content-Type", "application/json; charset=UTF-8");
    //response.setCookie(HttpCookie("firstCookie","hello",600,QByteArray(),QByteArray(),QByteArray(),false,true));
    //response.setCookie(HttpCookie("secondCookie","world",600));



    QByteArray reqinfo("");
    reqinfo.append("Request From Address:");
    reqinfo.append(request.getPeerAddress().toString());

    //handle CORS policy, check the method for every request.
    //if the method of request is OPTIONS reply with CORS response header.
    QByteArray method = request.getMethod();
    if(method.toUpper() == "OPTIONS")
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"收到OPTIONS数据";

        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        response.setHeader("Access-Control-Allow-Headers", "X-PINGOTHER, Content-Type");
        response.setHeader("Access-Control-Max-Age", "1728000");
        response.setStatus(204,"No Content");
        response.write("",true);
        return;
    }

    QByteArray body_j = request.getBody();
    QByteArray body_tosend ;

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body_j, &jsonError);

    /*
    QJsonObject qjSend,qjParams;
    QJsonObject qjData;

    qjParams["type"] = "2";
    qjParams["version"] = "48";
    qjParams["vehicle_brand"] = "6";
    qjSend["biz_id"] = "etc.f-issue.proof.apply";
    qjSend["params"] = qjParams;
    */

    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文解析错误,返回值 = "<<jsonError.error;
    }

    if (!jsonDoc.isObject())
    {
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"报文非Json格式数据";
    }

    QJsonObject jsonObject = jsonDoc.object();

    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    // For the following pathes, each request gets its own new instance of the related controller.

    if (path.endsWith("RSUPOpen"))
    {
        qDebug("RequestMapper: path=%s", "match");
        rsu_open(request, response, jsonObject);
        return;
    }
    else if (path.endsWith("RSUPClose"))
    {
        qDebug("RequestMapper: path=%s", "match");
        rsu_close(request, response, jsonObject);
        return;

    }
    else if (path.endsWith("RSUPInit"))
    {
        qDebug("RequestMapper: path=%s", "match");
        rsu_init(request, response, jsonObject);
        return;

    }
    else if (path.endsWith("TransferChannel"))
    {
        qDebug("RequestMapper: path=%s", "match");
        transfer_channel(request, response, jsonObject);
        return;

    }
    else if (path.endsWith("IssueFinishAndInitlalsation"))
    {
        qDebug("RequestMapper: path=%s", "match");
        issue_finish_and_initialization(request, response, jsonObject);
        return;

    }
    else if (path.endsWith("Initialization"))
    {
        qDebug("RequestMapper: path=%s", "match");
        init_obu(request, response, jsonObject);
        return;

    }
    else
    {
       response.setStatus(404,"Not Found");
       response.write("404 Not Found",true);
       return;
    }

    qDebug("RequestMapper: path=%s", "not match lhdf url, do nothing");
    //reqinfo.append(body_j);
    qWarning("Testing Warning");
    //qWarning(body_j);
    response.write(body_j,true);
}

void LhdfController::rsu_open(HttpRequest &request, HttpResponse &response, const QJsonObject& root)
{
    ERR_CODE err = ERR_NO_ERROR ;
    QJsonObject jsonObject;
    QString msgType, mode, dev, port;

    QJsonObject jsonResp;
    QByteArray bodySend ;

    do{
        if(!root.contains("msgType"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无msgType数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        msgType = root.value("msgType").toString();

        if(!root.contains("mode"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无mode数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        mode = root.value("mode").toString();

        dev = root.value("dev").toString();
        if("" == dev){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无dev数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

        if("1" == mode)
        {
            port = root.value("port").toString();
            if("" == port){
                qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无port数据项,返回值 = -1";
            }
            err = ERR_INVALID_REQUEST_PARA;
            break;

        }
    } while(0);

    if(ERR_NO_ERROR == err)
    {
       if(sdk_g->OpenRsu("COM"+dev) != 0)
           err = ERR_OPEN_DEVICE_FALIED;
    }
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"err code:" << err;
    QString flag;
    flag.sprintf("%05d", int(err));
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"flag:" << flag;
    jsonResp["flag"] = flag;
    jsonResp["msg"]  = get_errmsg_from_code(err);
    qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonResp).toJson();//qDebug().noquote()取消打印转义符号
    bodySend = QJsonDocument(jsonResp).toJson();

    //support CORS header.
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "X-PINGOTHER, Content-Type");
    response.write(bodySend,true);

}

void LhdfController::rsu_close(HttpRequest &request, HttpResponse &response, const QJsonObject &root)
{
    ERR_CODE err = ERR_NO_ERROR ;
    QJsonObject jsonObject;
    QString msgType;

    QJsonObject jsonResp;
    QByteArray bodySend ;

    do{

        if(!root.contains("msgType"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无msgType数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        msgType = root.value("msgType").toString();

    } while(0);

    if(ERR_NO_ERROR == err)
    {
       if(sdk_g->CloseRsu() != 0)
           err = ERR_OPEN_DEVICE_FALIED;
    }
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"err" << err;
    QString flag;
    flag.sprintf("%05d", int(err));
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"flag" << flag;
    jsonResp["flag"] = flag;
    jsonResp["msg"]  = get_errmsg_from_code(err);
    qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonResp).toJson();//qDebug().noquote()取消打印转义符号
    bodySend = QJsonDocument(jsonResp).toJson();
    response.write(bodySend,true);

}

void LhdfController::rsu_init(HttpRequest &request, HttpResponse &response, const QJsonObject &root)
{
    ERR_CODE err = ERR_NO_ERROR ;
    QJsonObject jsonObject;
    QString msgType, BSTInterval, RetryInterval, TxPower, PLLChannelID;

    QJsonObject jsonResp;
    QByteArray bodySend ;

    do{

        if(!root.contains("msgType"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无msgType数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        msgType = root.value("msgType").toString();

        if(!root.contains("BSTInterval"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无BSTInterval数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        BSTInterval = root.value("BSTInterval").toString();

        RetryInterval = root.value("RetryInterval").toString();
        if("" == RetryInterval){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无RetryInterval数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

        TxPower = root.value("TxPower").toString();
        if("" == TxPower){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无TxPower数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

        PLLChannelID = root.value("PLLChannelID").toString();
        if("" == PLLChannelID){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无PLLChannelID数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

    } while(0);

    char retinfo[256] = {0};
    int retlen = 256;

    jsonResp["RSUStatus"]  = "0";
    if(ERR_NO_ERROR == err)
    {
       if(sdk_g->InitRsu(nullptr, BSTInterval.toInt(), TxPower.toInt(), PLLChannelID.toInt(), 5000, retinfo, retlen) != 0){
           err = ERR_INIT_DEVICE_FALIED;
           jsonResp["RSUStatus"]  = "1";
       }
    }
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"err" << err;
    QString flag;
    flag.sprintf("%05d", int(err));
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"flag" << flag;
    jsonResp["flag"] = flag;
    jsonResp["msg"]  = get_errmsg_from_code(err);
    if(retlen > 0)
    {
        QString v = retinfo;
        jsonResp["RSUinfo"] = v;

    }else{

        jsonResp["RSUinfo"] = "";
    }

    qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonResp).toJson();//qDebug().noquote()取消打印转义符号
    if(ERR_NO_ERROR != err){
       response.setStatus(get_httpstatus_by_errcode(err), status_map[get_httpstatus_by_errcode(err)].toLatin1());
    }
    bodySend = QJsonDocument(jsonResp).toJson();

    // just test init obu here.
    char sEsamID[9] = {0},sObuID[17] = {0},sObuMac[9] = {0},sRandom[9] = {0};
    char sCosIns[128] = {0};
    sdk_g->OBUSearch(sObuMac);
    //end
    //support CORS
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "X-PINGOTHER, Content-Type");
    response.write(bodySend,true);
}

void LhdfController::transfer_channel(HttpRequest &request, HttpResponse &response, const QJsonObject &root)
{
    ERR_CODE err = ERR_NO_ERROR ;
    QJsonObject jsonObject;
    QString msgType, mode, DID, ChannelID, iAPDULIST, APDU ;

    QJsonObject jsonResp;
    QByteArray bodySend ;

    do{
        if(!root.contains("msgType"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无msgType数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        msgType = root.value("msgType").toString();

        if(!root.contains("mode"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无mode数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        mode = root.value("mode").toString();

        DID = root.value("DID").toString();
        if("" == DID){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无RetryInterval数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

        ChannelID = root.value("ChannelID").toString();
        if("" == ChannelID){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无TxPower数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

        iAPDULIST = root.value("iAPDULIST").toString();
        if("" == iAPDULIST){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无PLLChannelID数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

        APDU = root.value("APDU").toString();
        if("" == APDU){
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无PLLChannelID数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }

    } while(0);

    char retinfo[256] = {0};
    int retlen = 256;

    int mode_ = mode.toInt();
    int did_  = DID.toInt();
    int channel_id = ChannelID.toInt();
    int apdu_list  = iAPDULIST.toInt();
    int ret_status = 0;

    char apdu[128];
    char ret_apdu[256] = {0};
    //APDU.toLatin1().data()

    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"APDU:" << APDU;

    if(ERR_NO_ERROR == err)
    {
       if(sdk_g->TransferChannel(&mode_, &did_, &channel_id, &apdu_list, APDU.toLatin1().data(), ret_apdu, &ret_status) != 0){
           err = ERR_TRANSCHANNEL_FALIED;
           jsonResp["msg"]  = "1";
       }
    }
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"err" << err;
    QString flag;
    flag.sprintf("%05d", int(err));
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"flag" << flag;
    jsonResp["flag"] = flag;
    jsonResp["msg"]  = get_errmsg_from_code(err);

    jsonResp["DID"]          = QString().sprintf("%d", did_);
    jsonResp["ChannelID"]    = QString().sprintf("%d", channel_id);
    jsonResp["iAPDULIST"]    = QString().sprintf("%d", apdu_list);
    jsonResp["Date"]         = QString(ret_apdu);
    jsonResp["ReturnStatus"] = QString().sprintf("%d", ret_status);

    qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonResp).toJson();//qDebug().noquote()取消打印转义符号
    if(ERR_NO_ERROR != err){
       response.setStatus(get_httpstatus_by_errcode(err), status_map[get_httpstatus_by_errcode(err)].toLatin1());
    }
    bodySend = QJsonDocument(jsonResp).toJson();

    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "X-PINGOTHER, Content-Type");
    response.write(bodySend,true);
}

void LhdfController::issue_finish_and_initialization(HttpRequest &request, HttpResponse &response, const QJsonObject &root)
{
    ERR_CODE err = ERR_NO_ERROR ;
    QJsonObject jsonObject;
    QString msgType, apdu ;

    QJsonObject jsonResp;
    QByteArray bodySend ;

    do{
        if(!root.contains("msgType"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无msgType数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        msgType = root.value("msgType").toString();
        if(!root.contains("apdu"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无apdu数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        apdu = root.value("apdu").toString();
    } while(0);

    char retinfo[256] = {0};
    int retlen = 256;

    if(ERR_NO_ERROR == err)
    {
       if(sdk_g->OBURelease(1,1)){
           err = ERR_TRANSCHANNEL_FALIED;
       }
    }
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"err" << err;
    QString flag;
    flag.sprintf("%05d", int(err));
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"flag" << flag;
    jsonResp["flag"] = flag;
    //jsonResp["flag"] = "00001";
    jsonResp["msg"]  = get_errmsg_from_code(err);

    //jsonResp["resp"] = QString().sprintf("%s", "9000");
    jsonResp["resp"] = QString().sprintf("%s", "0001");

    qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonResp).toJson();//qDebug().noquote()取消打印转义符号
    if(ERR_NO_ERROR != err){
       response.setStatus(get_httpstatus_by_errcode(err), status_map[get_httpstatus_by_errcode(err)].toLatin1());
    }
    bodySend = QJsonDocument(jsonResp).toJson();
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "X-PINGOTHER, Content-Type");

/*
    // debug function if no new OBU found no response to the web browse.
    char sEsamID[9] = {0},sObuID[17] = {0},sObuMac[9] = {0},sRandom[9] = {0};
    char sCosIns[128] = {0};
    if( sdk_g->OBUSearch(sObuMac)){
          return;
    }
*/
    response.write(bodySend,true);
}

void LhdfController::init_obu(HttpRequest &request, HttpResponse &response, const QJsonObject &root)
{
    ERR_CODE err = ERR_NO_ERROR ;
    QJsonObject jsonObject;
    QString msgType, apdu ;

    QJsonObject jsonResp;
    QByteArray bodySend ;

    do{
        if(!root.contains("msgType"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无msgType数据项,返回值 = -1";
            err = ERR_INVALID_REQUEST_PARA;
            break;
        }
        msgType = root.value("msgType").toString();

    } while(0);

    char retinfo[256] = {0};
    int retlen = 256;

    char sEsamID[9] = {0},sObuID[17] = {0},sObuMac[9] = {0},sRandom[9] = {0};
    char sCosIns[128] = {0};

    sdk_g->OBUSearch(sObuMac);

    if(ERR_NO_ERROR == err)
    {
       if( sdk_g->OBUSearch(sObuMac)){
           err = ERR_TRANSCHANNEL_FALIED;
       }
    }
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"err" << err;
    QString flag;
    flag.sprintf("%05d", int(err));
    qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"flag" << flag;
    jsonResp["flag"] = flag;
    jsonResp["msg"]  = get_errmsg_from_code(err);

    qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonResp).toJson();//qDebug().noquote()取消打印转义符号
    if(ERR_NO_ERROR != err){
       response.setStatus(get_httpstatus_by_errcode(err), status_map[get_httpstatus_by_errcode(err)].toLatin1());
    }
    bodySend = QJsonDocument(jsonResp).toJson();
    response.write(bodySend,true);
}
