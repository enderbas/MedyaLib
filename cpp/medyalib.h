#ifndef MEDYALIB_H
#define MEDYALIB_H

#include "databasehelper.h"
#include <QListWidgetItem>
#include <QMainWindow>
#include <presentationwidget.h>
#include <badgetree.h>
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
	void on_checkShowFilters_stateChanged(int arg1);
	void on_toolSearch_clicked();

private:
	Ui::MedyaLib *ui;
	QString currentWorkPath;
	DatabaseHelper *dbHelper = nullptr;
	PresentationWidget *presentation = nullptr;
	BadgeTree *tree = nullptr;
private:
	void setCompleters();
	void addCompleter(QLineEdit *le, const QString &colName);
	void initBadgeTree();
};
#endif // MEDYALIB_H
