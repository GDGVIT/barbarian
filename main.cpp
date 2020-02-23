#include "barbarian.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    barbarian w;
    w.show();
    return a.exec();
}

