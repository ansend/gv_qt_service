/**
  @file
  @author Stefan Frings
*/

#include "formcontroller.h"
#include "./src/global.h"

FormController::FormController()
{}


void add_js(HttpResponse& response)
{
    //add java script
    response.write("<script>");
    response.write("function on_search_obu(){var y=document.forms[0].action; alert(y.value); y.value=\"searchobu\"; alert(y.value);document.main.submit();}");
    response.write("function on_close_obu() {var y=document.forms[0].action; alert(y.value); y.value=\"closeobu\"; alert(y.value);document.main.submit();}");

    response.write("</script>");
}


void FormController::service(HttpRequest& request, HttpResponse& response)
{

    response.setHeader("Content-Type", "text/html; charset=UTF-8");


    if(request.getMethod() == "GET"){



        response.write("<html><body>");

        add_js(response);

        response.write("<form name=\"main\" method=\"post\">");
        response.write("  <input type=\"hidden\" name=\"action\" value=\"show\">");
        response.write("  Name: <input type=\"text\" name=\"name\"><br>");
        response.write("  City: <input type=\"text\" name=\"city\"><br>");
        if (sdk_g->is_device_open()){
            response.write("  <input type=\"submit\" value=\"关闭设备\">");
        }else{
            response.write("  <input type=\"submit\" value=\"打开设备\">");
        }

        response.write("<br>");
        response.write("<span>");
        response.write("<input type=\"button\" onclick=\"on_search_obu()\" value=\"查找OBU\">");
        response.write("</span>");

        response.write("<br>");
        response.write("<span>");
        response.write("<input type=\"button\" onclick=\"on_close_obu()\" value=\"关闭OBU\">");
        response.write("</span>");


        response.write("</form>");

        response.write("<p>");
        if (sdk_g->is_device_open()){
            response.write("设备已经打开");
        }else{
            response.write("设备已经关闭");
        }
        response.write("</p>");
        response.write("</body></html>",true);


    }else{
        qDebug()<<"action value is = "<<request.getParameter("action");

        if (request.getParameter("action")=="show")
        {


            sdk_g->DoDevice();
            response.write("<html><body>");
            add_js(response);
            response.write("<form name=\"main\" method=\"post\">");
            response.write("  <input type=\"hidden\" name=\"action\" value=\"show\">");
            response.write("  Name: <input type=\"text\" name=\"name\"><br>");
            response.write("  City: <input type=\"text\" name=\"city\"><br>");
            if (sdk_g->is_device_open()){
                response.write("  <input type=\"submit\" value=\"关闭设备\">");
            }else{
                response.write("  <input type=\"submit\" value=\"打开设备\">");
            }

            response.write("<br>");
            response.write("<span>");
            response.write("<input type=\"button\" onclick=\"on_search_obu()\" value=\"查找OBU\">");
            response.write("</span>");


            response.write("<br>");
            response.write("<span>");
            response.write("<input type=\"button\" onclick=\"on_close_obu()\" value=\"关闭OBU\">");
            response.write("</span>");




            response.write("</form>");




            if (sdk_g->is_device_open()){

                response.write("设备已经打开");

            }else{
                response.write("设备已经关闭");
            }      response.write("<p>");

            response.write("</p>");
            response.write("</body></html>",true);
        }else if(request.getParameter("action")=="searchobu"){

            char sObuSerial[17] = {0},sAlgorithm[2] = {0},sKeyIndex[3] = {0},sKeyHead[7] = {0};
            char sMacData[11] = {0},sKeyMac[73] = {0},sProListNo[64] = {0},sListNo[64] = {0};
            char sEsamID[9] = {0},sObuID[17] = {0},sObuMac[9] = {0},sRandom[9] = {0};
            char sCosIns[128] = {0};

            memcpy(sObuSerial,"3701024099010202",16);
            memcpy(sProListNo,"3701012202101042142146292438548145518",37);
            memcpy(sAlgorithm,"1",1);

            int iRet = sdk_g->OBUSearch(sObuMac);

            sdk_g->start_testing();


            qDebug()<<"唤醒OBU,返回值 = "<<iRet;
            //if(0 != iRet) return;


            response.write("<html><body>");
            add_js(response);
            response.write("<form name=\"main\" method=\"post\">");
            response.write("  <input type=\"hidden\" name=\"action\" value=\"show\">");
            response.write("  Name: <input type=\"text\" name=\"name\"><br>");
            response.write("  City: <input type=\"text\" name=\"city\"><br>");
            if (sdk_g->is_device_open()){
                response.write("  <input type=\"submit\" value=\"关闭设备\">");
            }else{
                response.write("  <input type=\"submit\" value=\"打开设备\">");
            }

            response.write("<br>");
            response.write("<span>");
            response.write("<input type=\"button\" onclick=\"on_search_obu()\" value=\"查找OBU\">");
            response.write("</span>");


            response.write("<br>");
            response.write("<span>");
            response.write("<input type=\"button\" onclick=\"on_close_obu()\" value=\"关闭OBU\">");
            response.write("</span>");


            response.write("</form>");




            if (sdk_g->is_device_open()){

                response.write("设备已经打开");

            }else{
                response.write("设备已经关闭");
            }      response.write("<p>");

            response.write("</p>");
            response.write("</body></html>",true);

        }else if(request.getParameter("action")=="closeobu"){


            int iRet = sdk_g->OBURelease(1,1);
            qDebug()<<"关闭OBU,返回值 = "<<iRet;
            //if(0 != iRet) return;


            response.write("<html><body>");
            add_js(response);
            response.write("<form name=\"main\" method=\"post\">");
            response.write("  <input type=\"hidden\" name=\"action\" value=\"show\">");
            response.write("  Name: <input type=\"text\" name=\"name\"><br>");
            response.write("  City: <input type=\"text\" name=\"city\"><br>");
            if (sdk_g->is_device_open()){
                response.write("  <input type=\"submit\" value=\"关闭设备\">");
            }else{
                response.write("  <input type=\"submit\" value=\"打开设备\">");
            }

            response.write("<br>");
            response.write("<span>");
            response.write("<input type=\"button\" onclick=\"on_search_obu()\" value=\"查找OBU\">");
            response.write("</span>");


            response.write("<br>");
            response.write("<span>");
            response.write("<input type=\"button\" onclick=\"on_close_obu()\" value=\"关闭OBU\">");
            response.write("</span>");


            response.write("</form>");




            if (sdk_g->is_device_open()){

                response.write("设备已经打开");

            }else{
                response.write("设备已经关闭");
            }
            response.write("<p>");
            response.write("关闭OBU");
            response.write("</p>");
            response.write("</body></html>",true);



        }



    }


/*

    if (request.getParameter("action")=="show")
    {
        response.write("<html><body>");
        response.write("Name = ");
        response.write(request.getParameter("name"));
        response.write("<br>City = ");
        response.write(request.getParameter("city"));



        if (sdk_g->is_device_open()){
            response.write("  <input type=\"submit\" name=\"关闭设备\">");
        }else{
            response.write("  <input type=\"submit\" name=\"打开设备\">");
        }


        response.write("<p>");
        if (sdk_g->is_device_open()){
            response.write("设备已经打开");
        }else{
            response.write("设备已经关闭");
        }
        response.write("</p>");


        response.write("</body></html>",true);
    }
    else
    {
        sdk_g->DoDevice();
        response.write("<html><body>");
        response.write("<form method=\"post\">");
        response.write("  <input type=\"hidden\" name=\"action\" value=\"show\">");
        response.write("  Name: <input type=\"text\" name=\"name\"><br>");
        response.write("  City: <input type=\"text\" name=\"city\"><br>");
        response.write("  <input type=\"submit\">");
        response.write("</form>");

        if (sdk_g->is_device_open()){

            response.write("device is open");

        }else{
            response.write("device is close");
        }      response.write("<p>");

        response.write("</p>");
        response.write("</body></html>",true);
    }

  */

}

