/**
  @file
  @author ansendong
*/

#include "jsoncontroller.h"
#include <QVariant>
#include <QDateTime>
#include <QThread>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

JsonController::JsonController()
{}


QString GetFunLineInfor(const char* sFuncName,int iLineNum)
{
    return QString().sprintf("%-24s:%.05d:",sFuncName,iLineNum);
}


void JsonController::service(HttpRequest& request, HttpResponse& response)
{

    //response.setHeader("Content-Type", "text/html; charset=UTF-8");
    //response.setCookie(HttpCookie("firstCookie","hello",600,QByteArray(),QByteArray(),QByteArray(),false,true));
    //response.setCookie(HttpCookie("secondCookie","world",600));



    QByteArray reqinfo("");
    reqinfo.append("from address:");
    reqinfo.append(request.getPeerAddress().toString());


    QByteArray body_j = request.getBody();

    QByteArray body_tosend ;

    do {

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
            break;
        }

        if (!jsonDoc.isObject())
        {
            qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"报文非Json格式数据";
             break;
        }

        QJsonObject jsonObject = jsonDoc.object();
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QString(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);

        qDebug().noquote()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);//qDebug().noquote()取消打印转义符号

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson(QJsonDocument::Compact).data(); //取消打印转义符号

        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<QJsonDocument(jsonObject).toJson();
        qDebug()<< QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
        qDebug(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));


        QString mode = jsonObject.value("mode1").toString();
        qDebug()<<qPrintable(GetFunLineInfor(__FUNCTION__,__LINE__))<<"ansen mode value:" << mode;//qDebug().noquote()取消打印转义符号

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
