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

	enum Pages {
		GALLERY = 0,
		NEWMEDIA = 1
	};

public:
	MedyaLib(QWidget *parent = nullptr);
	~MedyaLib();
	struct MediaInfo {
		QString path;
		QString name;
		QString ext;
	} currentMediaInformation;

signals:
	void zoomIn();
	void zoomOut();

private slots:
	void open();
	void gallery();
	void on_toolSearch_clicked();
	void on_toolSaveInfos_clicked();
	void on_pushApplyFilters_clicked();

private:
	Ui::MedyaLib *ui;
	QString currentWorkPath;
	DatabaseHelper *dbHelper = nullptr;
	PresentationWidget *searchPres = nullptr;
	PresentationWidget *galleryPres = nullptr;
	BadgeTree *tree = nullptr;
	QAction *zoomInAct;
	QAction *zoomOutAct;

private:
	void setCompleters();
	void addCompleter(QLineEdit *le, const QString &colName);
	void initBadgeTree();
	void addToSearchBars(QLineEdit *line, const QStringList &badges);
	void createActions();
};
#endif // MEDYALIB_H
