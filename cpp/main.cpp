#include "medyalib.h"

#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MedyaLib w;
	w.resize(QDesktopWidget().availableGeometry().size() * 0.75);
	w.show();
	w.setWindowIcon(QIcon(":icons/workspaces_black_24dp.svg"));
	return a.exec();
}
