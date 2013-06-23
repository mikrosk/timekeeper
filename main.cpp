#include "timekeeper.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TimeKeeper w;

	QObject::connect( &a, SIGNAL( aboutToQuit() ), &w, SLOT( storeTimes() ) );

	w.show();
	return a.exec();
}
