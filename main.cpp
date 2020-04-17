#include "barbarian.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    barbarian w;
    if(system("conan > /dev/null") == 0) {
    w.show();
    return a.exec();
    }
}

