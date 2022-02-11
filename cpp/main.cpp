#include "medyalib.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	// https://youtu.be/EBGQUKFsGhA?t=1066
	// https://github.com/google/mediapipe
	QApplication a(argc, argv);
	MedyaLib w;
	w.show();
	return a.exec();
}
