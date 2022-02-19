#ifndef MEDYALIB_H
#define MEDYALIB_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "databasehelper.h"

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

private slots:
	void on_actionNew_Media_triggered();
	void on_actionGallery_triggered();
	void draw(QListWidgetItem *item);
	void on_toolSaveInfos_clicked();

private:
	Ui::MedyaLib *ui;
	QString currentWorkPath;
	QPair<QString, QString> currentMediaInformation;
	DatabaseHelper *dbHelper = nullptr;
private:
	void init();
	void setCompleters();
	void addCompleter(QLineEdit *le, const QString &colName);
};
#endif // MEDYALIB_H
