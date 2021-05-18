#ifndef LHDFCONTROLLER_H
#define LHDFCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

/**
  This controller dumps the received HTTP request in the response.
*/

class LhdfController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(LhdfController)
public:

    enum ERR_CODE{
        ERR_NO_ERROR             = 0,
        ERR_INVALID_REQUEST_PARA = 1,
        ERR_OPEN_DEVICE_FALIED,
        ERR_CLOSE_DEVICE_FALIED,
        ERR_INIT_DEVICE_FALIED,
        ERR_TRANSCHANNEL_FALIED


    };

    QString get_errmsg_from_code(ERR_CODE code)
    {
        QString ret = "Invalid Error Code";
        switch(code){
           case ERR_NO_ERROR:
              ret = "Success";
              break;
           case ERR_INVALID_REQUEST_PARA:
              ret = "Invalid Or Missing Reqeust Parameter";
            break;
           case ERR_OPEN_DEVICE_FALIED:
              ret = "Open Device Failed";
            break;
           case ERR_CLOSE_DEVICE_FALIED:
              ret = "Init Device Failed";
            break;
           case ERR_TRANSCHANNEL_FALIED:
             ret = "Transfer Channel Failed";
            break;
        default:
            break;

        }

        return ret;

    }

    int get_httpstatus_by_errcode(ERR_CODE code)
    {
        int ret = 200;
        switch(code){
           case ERR_NO_ERROR:
              ret = 200;
              break;
           case ERR_INVALID_REQUEST_PARA:
              ret =  400;
            break;
           case ERR_OPEN_DEVICE_FALIED:
           case ERR_CLOSE_DEVICE_FALIED:
           case ERR_INIT_DEVICE_FALIED:
           case ERR_TRANSCHANNEL_FALIED:
              ret = 500;
            break;
        default:
            break;
        }

        return ret;

    }
    /** Constructor */
    LhdfController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);

    //below is the interface handler for lhdf.
    void rsu_open(HttpRequest& request, HttpResponse& response, const QJsonObject& root);
    void rsu_close(HttpRequest& request, HttpResponse& response, const QJsonObject& root);
    void rsu_init(HttpRequest& request, HttpResponse& response, const QJsonObject& root);
    void transfer_channel(HttpRequest& request, HttpResponse& response, const QJsonObject& root);
    void issue_finish_and_initialization(HttpRequest& request, HttpResponse& response, const QJsonObject& root);
    void init_obu(HttpRequest& request, HttpResponse& response, const QJsonObject& root);

    static QMap<int, QString> status_map;
};


#endif // LHDFCONTROLLER_H
