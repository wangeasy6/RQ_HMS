#ifndef SMART_HOME_H
#define SMART_HOME_H

#include <QWidget>
#include <QTcpSocket>
#include <QThread>

#define Leave_Home 'L'
#define At_Home 'A'

#define CONNECTED 'I'
#define UN_CONNECT 'U'

#define ENV_ON "GET /?action=get_val"
#define CAMERA_ON "GET /?action=get_pic"

#define Max_Send_Length 1024

class WorkThread;

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
    void soltEnv();
    void soltcamera_on();
    void soltclose();
    void Camera();
    void ClickedAtHome();
    void ClickedLeaveHome();

public:
    QTimer *time;
    QTcpSocket *socket;
    QTcpSocket *s;
    char house_status;
    char connet_status;

    uint len = 0;
    char pic_data[250 * 1024] = {0};
    int pos = 0;

private:
    Ui::smart_home *ui;
};


#endif // SMART_HOME_H
