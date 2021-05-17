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


LhdfController::LhdfController()
{

}



void LhdfController::service(HttpRequest& request, HttpResponse& response)
{

    response.setHeader("Content-Type", "application/json; charset=UTF-8");
    //response.setCookie(HttpCookie("firstCookie","hello",600,QByteArray(),QByteArray(),QByteArray(),false,true));
    //response.setCookie(HttpCookie("secondCookie","world",600));



    QByteArray reqinfo("");
    reqinfo.append("Request From Address:");
    reqinfo.append(request.getPeerAddress().toString());


    QByteArray body_j = request.getBody();
    QByteArray body_tosend ;


    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body_j, &jsonError);


    QJsonObject qjSend,qjParams;
    QJsonObject qjData;


    qjParams["type"] = "2";
    qjParams["version"] = "48";

    qjParams["vehicle_brand"] = "6";

    qjSend["biz_id"] = "etc.f-issue.proof.apply";
    qjSend["params"] = qjParams;


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
    else if (path.endsWith("RSUPClose")){
        qDebug("RequestMapper: path=%s", "match");
        rsu_close(request, response, jsonObject);
        return;

    }


    qDebug("RequestMapper: path=%s", "not match");


    do {


        //qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QString(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));
        //qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);

        //qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);//qDebug().noquote()取消打印转义符号

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson(QJsonDocument::Compact).data();   // convert to c 'style' string 取消打印转义符号

       // qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson();
       // qDebug()<< QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
       // qDebug(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));



        if(!jsonObject.contains("response"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无response数据项,返回值 = -1";
            break;
        }

        jsonObject = jsonObject["response"].toObject();
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QString(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));

        if(!jsonObject.contains("area_no"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无code数据项,返回值 = -2";
             break;
        }

        int iRet = jsonObject["area_no"].toString().toInt();

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<< iRet;

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文area_no数据项返回返回值 = "<< jsonObject["area_no"].toString();



        iRet++;
        QString s = QString::number(iRet, 10);

        jsonObject["area_no"] = s;

        qjSend["area_no"] = s;

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QString(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));


        if(!jsonObject.contains("data"))
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"Json报文无data数据项,返回值 = -3";

        }

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__)) << QJsonDocument(jsonDoc.object()).toJson(QJsonDocument::Compact);
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__)) << jsonDoc.toJson(QJsonDocument::Compact);


         body_tosend = QJsonDocument(qjSend).toJson(QJsonDocument::Compact);
         qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__)) << body_tosend;

    }while (0);








    reqinfo.append(body_j);
    qWarning("Testing Warning");

    qWarning(body_j);

    QString str="world!";

    qWarning(str.toUtf8() + "hello world");


    QByteArray body("<html><body>");
    body.append("<b>Request:</b>");
    body.append("<br>Method: ");
    body.append(request.getMethod());
    body.append("<br>Path: ");
    body.append(request.getPath());
    body.append("<br>Version: ");
    body.append(request.getVersion());

    body.append("<p><b>Headers:</b>");
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QMultiMapIterator<QByteArray,QByteArray> i(request.getHeaderMap());
    #else
        QMapIterator<QByteArray,QByteArray> i(request.getHeaderMap());
    #endif
    while (i.hasNext())
    {
        i.next();
        body.append("<br>");
        body.append(i.key());
        body.append("=");
        body.append(i.value());
    }

    body.append("<p><b>Parameters:</b>");

    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        i=QMultiMapIterator<QByteArray,QByteArray>(request.getParameterMap());
    #else
        i=QMapIterator<QByteArray,QByteArray>(request.getParameterMap());
    #endif
    while (i.hasNext())
    {
        i.next();
        body.append("<br>");
        body.append(i.key());
        body.append("=");
        body.append(i.value());
    }

    body.append("<p><b>Cookies:</b>");
    QMapIterator<QByteArray,QByteArray> i2 = QMapIterator<QByteArray,QByteArray>(request.getCookieMap());
    while (i2.hasNext())
    {
        i2.next();
        body.append("<br>");
        body.append(i2.key());
        body.append("=");
        body.append(i2.value());
    }

    body.append("<p><b>Body:</b><br>");
    body.append(request.getBody());

    body.append("</body></html>");
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
    QString msgType, BSTInterval, RetryInterval, TxPower, PLLChannelID

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

    if(ERR_NO_ERROR == err)
    {
       if(sdk_g->OpenRsu("COM"+dev) != 0)
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
