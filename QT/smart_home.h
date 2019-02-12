#ifndef SMART_HOME_H
#define SMART_HOME_H

#include <QWidget>
#include <QSound>
#include <QtMultimedia/QMediaPlayer>
#include <QTcpSocket>
#include <QThread>

//  配置
#define Leave_Home 'L'
#define At_Home 'A'

#define CONNECTED 'I'
#define UN_CONNECT 'U'

#define Max_Send_Length 8*1024

#define FIRE_V 29

#define SAVE_FILE 0     // 0|1
#define Debug_Con 0

namespace Ui {
class smart_home;
}

class smart_home : public QWidget
{
    Q_OBJECT
    Ui::smart_home *ui;

public:
    explicit smart_home(QWidget *parent = 0);
    ~smart_home();


protected slots:
    void soltLog();
    void soltConnected();
    void soltRecv();
    void soltClose();
    void clickedAtHome();
    void clickedLeaveHome();
    void checkedBT();

private:
    QTcpSocket *socket;
    char house_status;
    char connet_status;
    QSound *s_alarm;
    QSound *s_dingdong;
    QSound *s_enter;
    int temp_h = FIRE_V;    // 温度报警阈值
    QString log;

    int len = 0;
    uint pos = 0;
    char pic_data[250 * 1024] = {0};

#if SAVE_FILE
    FILE *cap_pic;
#endif
};


#endif // SMART_HOME_H
