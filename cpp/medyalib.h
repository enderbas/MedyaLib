#ifndef MEDYALIB_H
#define MEDYALIB_H

#include "databasehelper.h"
#include <QListWidgetItem>
#include <QMainWindow>
#include <presentationwidget.h>
QT_BEGIN_NAMESPACE
namespace Ui
{
class MedyaLib;
}
QT_END_NAMESPACE

class MedyaLib : public QMainWindow
{
	Q_OBJECT

public:
	MedyaLib(QWidget *parent = nullptr);
	~MedyaLib();
	struct MediaInfo {
		QString path;
		QString name;
		QString ext;
	} currentMediaInformation;

private slots:
	void on_actionNew_Media_triggered();
	void on_actionGallery_triggered();
	void on_toolSaveInfos_clicked();

private:
	Ui::MedyaLib *ui;
	QString currentWorkPath;
	DatabaseHelper *dbHelper = nullptr;
	PresentationWidget *presentation = nullptr;

private:
	void setCompleters();
	void addCompleter(QLineEdit *le, const QString &colName);
};
#endif // MEDYALIB_H
