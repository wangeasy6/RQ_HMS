#include "smart_home.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    smart_home w;
    w.show();

    return a.exec();
}
