#include "medyalib.h"
#include "./ui_medyalib.h"
#include <QCompleter>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
MedyaLib::MedyaLib(QWidget *parent) : QMainWindow(parent), ui(new Ui::MedyaLib)
{
	ui->setupUi(this);
	presentation = new PresentationWidget(this);
	ui->layoutSplitter->addWidget(presentation);

	dbHelper = new DatabaseHelper(QDir::currentPath(), "medyalib.db");
	setCompleters();
}

MedyaLib::~MedyaLib()
{
	delete ui;
}

void MedyaLib::on_actionNew_Media_triggered()
{
	ui->stackedWidget->setCurrentIndex(1);
	QFileDialog fileDialog(this);
	fileDialog.setFileMode(QFileDialog::Directory);
	fileDialog.setOptions(QFileDialog::ShowDirsOnly |
						  QFileDialog::DontResolveSymlinks);
	QDir directory(fileDialog.getExistingDirectory());
	QStringList medias = directory.entryList(QStringList() << "*.jpg"
														   << "*.JPG"
														   << "*.png"
														   << "*.PNG",
											 QDir::Files);
	presentation->showListedItems(medias, directory);
}

void MedyaLib::on_actionGallery_triggered()
{
	ui->stackedWidget->setCurrentIndex(0);
}

void MedyaLib::setCompleters()
{
	addCompleter(ui->lineEditWho, "persons");
	addCompleter(ui->lineEditTimes, "dates");
	addCompleter(ui->lineEditTags, "tags");
	addCompleter(ui->lineEditLocat, "locations");
}

void MedyaLib::addCompleter(QLineEdit *le, const QString &colName)
{
	if (!dbHelper || !le) {
		qWarning() << "Completer could not applied.";
		return;
	}
	QCompleter *completer =
		new QCompleter(dbHelper->getColumnItems(colName), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	le->setCompleter(completer);
}

void MedyaLib::on_toolSaveInfos_clicked()
{
	auto strMap = dbHelper->getFieldStrings();

	QString mediaId;
	if (currentMediaInformation.name.size())
		mediaId = dbHelper->addMediaData(currentMediaInformation.name,
										 currentMediaInformation.ext);
	QString id = "";
	if (ui->lineEditWho->text().size()) {
		id = dbHelper->addData(strMap.find(DatabaseHelper::PERSONS).value(),
							   ui->lineEditWho->text());
		dbHelper->addMediaCombinedData("media_persons", id, mediaId);
	}
	if (ui->lineEditTimes->text().size()) {
		id = dbHelper->addData(strMap.find(DatabaseHelper::DATES).value(),
							   ui->lineEditTimes->text());
		dbHelper->addMediaCombinedData("media_dates", id, mediaId);
	}
	if (ui->lineEditTags->text().size()) {
		id = dbHelper->addData(strMap.find(DatabaseHelper::TAGS).value(),
							   ui->lineEditTags->text());
		dbHelper->addMediaCombinedData("media_tags", id, mediaId);
	}
	if (ui->lineEditLocat->text().size()) {
		id = dbHelper->addData(strMap.find(DatabaseHelper::LOCATIONS).value(),
							   ui->lineEditLocat->text());
		dbHelper->addMediaCombinedData("media_locations", id, mediaId);
		addCompleter(ui->lineEditLocat,
					 strMap.find(DatabaseHelper::LOCATIONS).value());
	}
	if (currentMediaInformation.path.size()) {
		id = dbHelper->addData(strMap.find(DatabaseHelper::PATHS).value(),
							   currentMediaInformation.path);
		dbHelper->addMediaCombinedData("media_paths", id, mediaId);
	}
}
