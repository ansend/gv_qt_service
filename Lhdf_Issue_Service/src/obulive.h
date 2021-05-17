#ifndef OBULIVE_H
#define OBULIVE_H


#include <QtCore/QThread>
class QTimer ;
class MyThread : public QThread
{
    Q_OBJECT

public:
    MyThread();
    ~MyThread();
protected:
    void run() ;

public:
    static bool enable_flag;
    static int curr_state;  //1 for enable 0 for disable.
public slots:
    void slot_time() ;
    void start_timer();
    void stop_timer();

signals:
    void sendtime(QString str) ;
private:
    QTimer *m_timer ;
};




#endif // OBULIVE_H
