#include "obulive.h"
#include "global.h"
#include <QtCore/QTime>
#include <QtCore/QTimer>

bool MyThread::enable_flag = false;
int MyThread::curr_state = 0;

MyThread::MyThread()
    : QThread()
{

}

MyThread::~MyThread()
{

}


void MyThread::run()
{
    m_timer = new QTimer ;
    connect(m_timer , SIGNAL(timeout()) , this , SLOT(slot_time())) ;
    m_timer->setInterval(200);

    while(true)
    {
        //qWarning("loop timer");
        if(enable_flag)
        {
           //qWarning("trigger timer");
           //sdk_g->DoObuKeepOnPowerTimeOutHandle();
           curr_state = 1;

        }
        QThread::msleep(200);

    }

}



/*
void MyThread::run()
{
    m_timer = new QTimer ;
    connect(m_timer , SIGNAL(timeout()) , this , SLOT(slot_time())) ;
    m_timer->setInterval(200);



    this->exec();

}
*/


void MyThread::slot_time()
{
   qWarning("trigger timer");
   sdk_g->DoObuKeepOnPowerTimeOutHandle();
}


void MyThread::start_timer()
{
    qWarning("start timer in MyThread");
    if(!m_timer->isActive())
    {
       m_timer->start();
    }
}

void MyThread::stop_timer()
{
   qWarning("stop timer in MyThread");
   if(m_timer->isActive())
   {
      m_timer->stop();
   }
}

