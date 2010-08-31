#include <QtGui/QApplication>
#include "adserversimulator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AdServerSimulator w;
    w.show();

    return a.exec();
}
