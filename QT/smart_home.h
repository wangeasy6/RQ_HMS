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

#define Max_Send_Length 1024

#define FIRE_V 29

namespace Ui {
class smart_home;
}

class smart_home : public QWidget
{
    Q_OBJECT

public:
    explicit smart_home(QWidget *parent = 0);
    ~smart_home();
    void camera_pix();


protected slots:
    void SOLTlog();
    void soltConnected();
    void soltRecv();
    void soltclose();
    void ClickedAtHome();
    void ClickedLeaveHome();
    void checked_i();

public:
    QTimer *time;
    QTcpSocket *socket;
    char house_status;
    char connet_status;
    QSound *s_alarm;
    QSound *s_dingdong;
    QSound *s_enter;

    uint len = 0;
    char pic_data[250 * 1024] = {0};
    int pos = 0;
    int temp_h = FIRE_V;

private:
    Ui::smart_home *ui;
};


#endif // SMART_HOME_H
