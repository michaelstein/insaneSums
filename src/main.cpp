#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a( argc, argv );
	MainWindow w( nullptr, (Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint) & ~Qt::WindowMaximizeButtonHint );
	w.show();
	return a.exec();
}