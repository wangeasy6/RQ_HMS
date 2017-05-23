#include "smart_home.h"
#include "ui_smart_home.h"

#include <QtGui>
#include <QPushButton>
#include <QDateTime>
#include <QHostAddress>
#include <QtDebug>
#include <QString>
#include <stdlib.h>
#include <QtNetwork/QTcpSocket>
#include <QPixmap>
#include <QIcon>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QPixmap>

smart_home::smart_home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::smart_home)
{
    ui->setupUi(this);
    this->setWindowTitle("智能家居监控系统");
    this->time = new QTimer(this);
    // 设置能显示的位数
    ui->HUM_M->setDigitCount(3);
    // 设置显示的模式为十进制
    ui->HUM_M->setMode(QLCDNumber::Dec);
    // 设置显示外观
    ui->HUM_M->setSegmentStyle(QLCDNumber::Flat);
    // 设置样式
    ui->HUM_M->setStyleSheet("border: 1px solid green; color: green; background: silver;");
    ui->TEM_M->setDigitCount(3);
    ui->TEM_M->setMode(QLCDNumber::Dec);
    ui->TEM_M->setSegmentStyle(QLCDNumber::Flat);
    ui->TEM_M->setStyleSheet("border: 1px solid green; color: green; background: silver;");

    ui->IP->setText("169.254.8.181");

    ui->Checked->setEnabled(false);
    connect(ui->Checked, SIGNAL(clicked(bool)),
            this, SLOT(checked_bt()) );
    //connect();
    connect(ui->AtHome,SIGNAL(clicked()),
            this,SLOT(ClickedAtHome()));

    connect(ui->LeaveHome,SIGNAL(clicked()),
            this,SLOT(ClickedLeaveHome()));

    ui->connect->setEnabled(false);
    ui->disconnect->setEnabled(false);
    ui->fired->setStyleSheet("color:red");
    ui->fired->setVisible(false);

    ui->record->setReadOnly(true);

    connet_status = UN_CONNECT;

    connect(ui->connect, SIGNAL(clicked()),
                this, SLOT(SOLTlog()));
    connect(ui->disconnect,SIGNAL(clicked(bool)),
                this,SLOT(soltclose()));
    s_alarm = new QSound(":/sound/alarm.wav", Q_NULLPTR);
    s_dingdong = new QSound(":/sound/dingdong.wav",Q_NULLPTR);
    s_enter = new QSound(":/sound/enter.wav",Q_NULLPTR);

    /*      file test
    FILE *file_test = fopen("file_test.txt","w+");
    fwrite("file ",sizeof("file "),1,file_test);
    fwrite("test",sizeof("test"),1,file_test);
    fclose(file_test);
    */
}
smart_home::~smart_home()
{
    delete ui;
}
/********************************************************* Init ***************************************************/

void smart_home::ClickedAtHome()
{
    house_status = At_Home;
    if(connet_status == UN_CONNECT && !ui->connect->isEnabled())
        ui->connect->setEnabled(true);
    return;
}

void smart_home::ClickedLeaveHome()
{
    house_status = Leave_Home;

    if(connet_status == UN_CONNECT && !ui->connect->isEnabled())
        ui->connect->setEnabled(true);

    return;
}

/****************************************************** hose_status ***********************************************/

void smart_home::SOLTlog()
{
    bool ok;
    int num = ui->Port->text().toInt(&ok,10);
    socket = new QTcpSocket(this);
    connect(this->socket, SIGNAL(connected()),
           this, SLOT(soltConnected()));
   socket->connectToHost(QHostAddress(ui->IP->text()),num);
}

void smart_home::soltConnected()
{
    qDebug()<<"成功连接服务器";
    ui->connect->setEnabled(false);
    ui->disconnect->setEnabled(true);

    connet_status = CONNECTED;

    connect(socket, SIGNAL(readyRead()),
                this, SLOT(soltRecv()));

}

void smart_home::soltRecv()
{
    char data[Max_Send_Length];
    qint64 ret;
    QString String;
    QStringList ENV;
    QPixmap pixmap;

    while(1)
    {
        ret = socket->read(data,Max_Send_Length);
        //ret = socket->readLine(data,1500);
        if(-1 == ret || 0 == ret)
            break;
        #if SAVE_FILE
                if( len > 0 && ret != 2 && ret != 6)        //直接保存到文件
                {
                    memcpy( pic_data+pos, data, ret );
                    len -= ret;
                    pos += ret;
/*
                    char buf[20];
                    snprintf(buf, sizeof(buf), "%x %x %x ", data[0]&0xFF, data[1]&0xFF, data[2]&0xFF);
                    qDebug() << "ret.save.PIC: " << ret << "len" << len << " pos:" << pos << "buf:" << buf <<endl;
*/
                    //fwrite(  data, ret, 1, cap_pic );
                    if(len <= 0)
                    {
                        fwrite(  pic_data, pos, 1,cap_pic );
                        fclose(cap_pic);
                        //qDebug()<<"save Pixmap\n";
                        memset(pic_data, 0, sizeof(pic_data));
                        pos = 0;
                        len = 0;
                    }
                    continue;
                }
        #else
                if( len > 0 && ret != 2 && ret != 6)        //直接显示到界面
                {
                    memcpy( pic_data+pos, data, ret );
                    len -= ret;
                    pos += ret;
                    //qDebug() << "ret.PIC: " << ret << "len" << len << " pos:" << pos <<"\r\n";
                    if(len <= 0)
                    {
                        QPixmap *pix = new QPixmap(320,240);
                        pix->loadFromData( (uchar*)pic_data, pos, "JPEG" );
                        ui->Video->setPixmap( *pix );
                        qDebug()<<"set Pixmap\n";
                        memset(pic_data, 0, sizeof(pic_data));
                        pos = 0;
                        len = 0;
                    }
                    continue;
                }
        #endif
        switch(data[0])
        {
            case 'E'://温湿度采集
            //qDebug() << "error data:" << data << "\r\n";
                for(int i = 0;i < 5;i++)
                {
                    String[i] = QChar(data[i+1]);
                }
                qDebug() << String;
                ENV = String.split(',');

                ui->HUM_M->display(ENV[0].toInt());
                ui->TEM_M->display(ENV[1].toInt());
                if( ENV[1].toInt() > temp_h )
                {
                    if( s_alarm->isFinished() )
                    {
                        s_alarm->play();
                    }
                    ui->fired->setVisible(true);
                }else{
                    s_alarm->stop();
                    ui->fired->setVisible(false);
                }
            break;
            case 'I'://红外检测
            //qDebug() << "error data:" << data << "\r\n";
                if('1' == data[1])
                {
                    ui->Checked->setEnabled(true);
                    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
                    if(house_status == At_Home)
                    {
                        log.append("You have a visitor , " + time.toString("yyyy-MM-dd hh:mm:ss") + "\r\n");
                        ui->record->setText(log);
                        if( !pixmap.load(":/jpg/green.png",Q_NULLPTR,Qt::AutoColor) )
                            qDebug() << "load green.png error\n";
                        if( s_dingdong->isFinished() )
                        {
                            s_dingdong->setLoops(3);
                            s_dingdong->play();
                        }
                    }
                    if(house_status == Leave_Home)
                    {
                        log.append("Somebody broke in, " + time.toString("yyyy-MM-dd hh:mm:ss") + "\r\n");
                        ui->record->setText(log);
                        if( !pixmap.load(":/jpg/red.png",Q_NULLPTR,Qt::AutoColor) )
                            qDebug() << "load green.png error\n";
                        if( s_enter->isFinished() )
                        {
                            s_enter->setLoops(100);
                            s_enter->play();
                        }
                    }
                    ui->Light->setPixmap(pixmap);
                }
            break;
            case 'H'://Header
            {
                QString *length = new QString( data+1 );
                len = length->toUInt(Q_NULLPTR,10);
                pos = 0;
                memset( pic_data, 0 ,sizeof(pic_data));
                qDebug()<<"ret.H:"<< ret <<" ,Header_len:"<< len <<"\n";
                if(ret > 10)
                {
                    ret -= 10;
                    memcpy( pic_data+pos, data+10, ret );
                    len -= ret;
                    pos += ret;
                    char buf[20];
                    snprintf(buf, sizeof(buf), "%x %x %x ", data[10]&0xFF, data[11]&0xFF, data[12]&0xFF);
                    qDebug() << "ret.save.PIC: " << ret << "len" << len << " pos:" << pos << "buf:" << buf <<endl;
                }
        #if SAVE_FILE
                cap_pic = fopen("cap_picture.jpg", "wb+");
                if(!cap_pic) //读取失败 if(!fp) 与 if(fp == NULL) 等价
                {
                    qDebug()<< "fopen err";
                    return;
                }
        #endif
            }
            break;
            default :
            break;
        }
    }
}

void smart_home::checked_bt()
{

    if( !s_dingdong->isFinished() )
        s_dingdong->stop();
    if( !s_enter->isFinished() )
        s_enter->stop();
    ui->Light->clear();
    ui->Checked->setEnabled(false);
}

void smart_home::soltclose()
{
    ui->connect->setEnabled(true);
    ui->disconnect->setEnabled(false);
    socket->close();
    connet_status = UN_CONNECT;
    ui->HUM_M->display(0);
    ui->TEM_M->display(0);
    len = 0;
    log.clear();
    ui->record->clear();
    ui->Light->clear();
    ui->fired->setVisible(false);
    if( !s_alarm->isFinished() )
        s_alarm->stop();
    if( !s_dingdong->isFinished() )
        s_dingdong->stop();
    if( !s_enter->isFinished() )
        s_enter->stop();

#if SAVE_FILE
    if( cap_pic != NULL)
        fclose(cap_pic);
#endif
}

/******************************************** connect && recv_data && close ****************************************/
