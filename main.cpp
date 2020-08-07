#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QStyle>

#include <src/runguard.h>

// https://doc.qt.io/qt-5/qtwidgets-widgets-windowflags-example.html
// https://stackoverflow.com/questions/29803232/how-to-hide-application-on-taskbar
int main(int argc, char *argv[])
{
	RunGuard guard( "lunremote-server" );
	if (!guard.tryToRun())
		return 0;

	QApplication a(argc, argv);
	QApplication::setQuitOnLastWindowClosed(false);
	QFontDatabase::addApplicationFont(":/fonts/ubuntu/light");
	QFontDatabase::addApplicationFont(":/fonts/ubuntu/medium");
	QIcon icon = QIcon(":/icons/remocy-icon");
	MainWindow w;
	w.hide();
//    w.setWindowIcon(icon);
//    w.show();
//    a.setWindowIcon(icon);
    return a.exec();
}

