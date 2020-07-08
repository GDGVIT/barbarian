#include "barbarian.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    barbarian w;
	QProcess checkIfi;
	QStringList checkIfListi = {">", "/dev/null"};
	checkIfi.start("conan", checkIfListi, QIODevice::ReadWrite);
	checkIfi.waitForFinished(-1);
    if(checkIfi.exitCode() == 0) {
	    w.show();
    	return a.exec();
    }
}

