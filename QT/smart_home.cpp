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

char buf1[100*1024];
char buf[15];

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

    ui->IP->setText("192.168.3.107");

    ui->Checked->setEnabled(false);
    //connect();
    connect(ui->AtHome,SIGNAL(clicked()),
            this,SLOT(ClickedAtHome()));

    connect(ui->LeaveHome,SIGNAL(clicked()),
            this,SLOT(ClickedLeaveHome()));

    ui->connect->setEnabled(false);
    ui->disconnect->setEnabled(false);

    ui->record->setReadOnly(true);

    connet_status = UN_CONNECT;

    connect(ui->connect, SIGNAL(clicked()),
                this, SLOT(SOLTlog()));
    connect(ui->disconnect,SIGNAL(clicked(bool)),
                this,SLOT(soltclose()));
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
#ifdef MULTI_THREADING
    s = new QTcpSocket(this);
    connect(this->s,SIGNAL(connected()),
            this,SLOT(soltConnect()));
    s->connectToHost(QHostAddress(ui->lineEdit->text()),num);
#endif
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
            break;
            case 'I'://红外检测
            //qDebug() << "error data:" << data << "\r\n";
                if('1' == data[1])
                {
                    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
                    if(house_status == At_Home)
                    {
                        QString str = "You have a visitor , ";
                        str.append(time.toString("yyyy-MM-dd hh:mm:ss"));
                        ui->record->setText(str);
                        if( !pixmap.load(":/jpg/green.png",Q_NULLPTR,Qt::AutoColor) )
                            qDebug() << "load green.png error\n";
                    }
                    if(house_status == Leave_Home)
                    {
                        QString str = "Somebody broke in , ";
                        str.append(time.toString("yyyy-MM-dd hh:mm:ss"));
                        ui->record->setText(str);
                        if( !pixmap.load(":/jpg/red.png",Q_NULLPTR,Qt::AutoColor) )
                            qDebug() << "load green.png error\n";
                    }
                    ui->Light->setPixmap(pixmap);
                }
            break;
            case 'H'://Header
            {
                QString *length = new QString( data+1 );
                len = length->toUInt(Q_NULLPTR,10);
                pos =0;
                qDebug()<<"ret.H:"<< ret <<" ,Header_len:"<< len <<"\n";
            }
            break;
            case 'B'://半包
            {
                ret -= 1;
                len -= ret;
                pos += ret;
                memcpy( pic_data+pos, data+1, ret );
                qDebug() << "ret.B: " << ret << "len" << len << " pos:" << pos << "\r\n";
                if(len <= 0)
                {
                    QPixmap *pix = new QPixmap(320,240);
                    pix->loadFromData( (uchar*)pic_data, pos, "JPEG" );
                    ui->Video->setPixmap( *pix );
                    qDebug()<<"setPixmap\n";
                    memset(pic_data, 0, sizeof(pic_data));
                    pos = 0;
                }
            }
            break;
            default :
            {
                len -= ret;
                pos += ret;
                memcpy( pic_data+pos, data, ret );
                if(len <= 0)
                {
                    //QPixmap *pix = new QPixmap(320,240);
                    QPixmap *pix;
                    pix->loadFromData( (uchar*)pic_data, pos, "JPEG" );
                    ui->Video->setPixmap( *pix );
                    qDebug()<<"setPixmap\n";
                    memset(pic_data, 0, sizeof(pic_data));
                    pos = 0;
                }
                char debug_data[10] = {0};
                for (int i=0;i<9;i++)
                    debug_data[i] = data[i];
                qDebug() << "ret.ERR: " << ret << "len" << len << " pos:" << pos << "data:"<< debug_data <<"\r\n";
            }
               // qDebug() << "ret.ERR: " << data << "\r\n";
            break;
        }
    }
}

void smart_home::soltclose()
{
    ui->connect->setEnabled(true);
    ui->disconnect->setEnabled(false);
    this->camera_pix();
    socket->close();
    connet_status = UN_CONNECT;
    ui->HUM_M->display(0);
    ui->TEM_M->display(0);
    ui->record->clear();
    ui->Light->clear();
#ifdef MULTI_THREADING
    s->close();
#endif
}

/******************************************** connect && recv_data && close ****************************************/

void smart_home::soltcamera_on()
{
    /*
    switch (this->camera_status) {
    case CAMERA_false:
        ui->camera_on->setText("关闭摄像头");
        this->time->start(1000);
        this->camera_status = CAMERA_true;
        break;
    case CAMERA_true:
        ui->camera_on->setText("开启摄像头");
        this->time->stop();
        this->camera_status = CAMERA_false;
        this->camera_pix();
        break;
    }
    */
    return;
}

void smart_home::soltEnv()
{
    QString str = "";
    QString string;
    ui->record->setText(str);  //清空文本框
    strcpy(buf,ENV_ON);
    s->write(buf,strlen(buf));
    s->flush();
    memset(buf,0,1024);
    s->read(buf,1024);
    if(buf != 0)
    {
        string.append(buf);
        str += string;
    }

    ui->record->setText(str);
}
void smart_home::Camera()
{
    memset(buf,0,15);
    strcpy(buf,CAMERA_ON);
    qDebug()<<buf<<endl;
    s->write(buf,strlen(buf));
    s->flush();
    s->read(buf,6);
    int num = atoi(buf);
    int count = num;
    while(count>0)
    {
        int ret = s->read(buf1,num);
        count =count - ret;
    }
    //char *buf2="GET /?getpic=1.jpg";
    QPixmap pixmap;
    pixmap.loadFromData((uchar *)buf1,num, "JPEG");
    //pixmap.loadFromData((uchar *)buf2,num, "JPEG");
    //ui->Video->setPixmap(pixmap);

}

void smart_home::camera_pix()
{
    QPixmap pix(":/jpg/white.jpg");
    //ui->Video->setPixmap(pix);
}


























