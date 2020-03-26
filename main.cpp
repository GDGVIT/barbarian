#include "barbarian.h"

#include <QApplication>

int main(int argc, char *argv[])
{
<<<<<<< HEAD

    QApplication a(argc, argv);
    barbarian w;
    if(system("conan > /dev/null") == 0) {
    w.show();
    return a.exec();
    }
=======
    QApplication a(argc, argv);
    barbarian w;
    w.show();
    return a.exec();
>>>>>>> f792dea0103330dac4bf35032dc2fb13f4df1e3e
}

