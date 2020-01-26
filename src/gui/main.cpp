#include <QApplication>
#include "DisplayWidget.h"


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	DisplayWidget w;
	w.show();
	return app.exec();
}
