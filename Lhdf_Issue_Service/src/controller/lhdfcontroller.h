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
        ERR_CLOSE_DEVICE_FALIED


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
              ret = "Open Device Failed";
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
};


#endif // LHDFCONTROLLER_H
