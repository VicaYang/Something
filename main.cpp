#include "something.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Something w;
    w.show();

    return a.exec();
}
