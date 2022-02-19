#include "medyalib.h"
#include "./ui_medyalib.h"
#include <QCompleter>
#include <QDebug>
#include <QFileDialog>
#include <QImageReader>
MedyaLib::MedyaLib(QWidget *parent) : QMainWindow(parent), ui(new Ui::MedyaLib)
{
	ui->setupUi(this);
	init();
	ui->thumbnailNewImages->setViewMode(QListWidget::IconMode);
	ui->thumbnailNewImages->setIconSize(QSize(200, 150));
	ui->thumbnailNewImages->setResizeMode(QListWidget::Adjust);
	// https://stackoverflow.com/questions/26829754/how-to-use-the-threads-to-create-the-images-thumbnail
	//TODO: draw images using threads
	connect(ui->thumbnailNewImages, SIGNAL(itemClicked(QListWidgetItem *)),
			this, SLOT(draw(QListWidgetItem *)));
	dbHelper = new DatabaseHelper("F:/source/MedyaLib2/DB/test.db");
	dbHelper->createDb("test");
	setCompleters();
}

MedyaLib::~MedyaLib()
{
	delete ui;
}

void MedyaLib::on_actionNew_Media_triggered()
{
	ui->stackedWidget->setCurrentIndex(1);
	ui->thumbnailNewImages->clear();
	QFileDialog fileDialog(this);
	fileDialog.setFileMode(QFileDialog::Directory);
	fileDialog.setOptions(QFileDialog::ShowDirsOnly |
						  QFileDialog::DontResolveSymlinks);
	currentWorkPath = fileDialog.getExistingDirectory();
	QDir directory(currentWorkPath);
	QStringList images = directory.entryList(QStringList() << "*.jpg"
														   << "*.JPG"
														   << "*.png"
														   << "*.PNG",
											 QDir::Files);
	for (const auto &val : qAsConst(images)) {
		QString path = directory.absoluteFilePath(val);
		QListWidgetItem *item = new QListWidgetItem(QIcon(path), val);
		ui->thumbnailNewImages->addItem(item);
	}
}

void MedyaLib::on_actionGallery_triggered()
{
	ui->stackedWidget->setCurrentIndex(0);
}

void MedyaLib::draw(QListWidgetItem *item)
{
	QString path = currentWorkPath + "/" + item->text();
	QImageReader reader(path);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	if (newImage.isNull()) {
		return;
	}
	currentMediaInformation.first = currentWorkPath;
	currentMediaInformation.second = item->text();
	ui->labelImage->clear();
	ui->labelImage->setPixmap(QPixmap::fromImage(newImage));
}

void MedyaLib::init()
{
	ui->labelImage->setBackgroundRole(QPalette::Base);
	ui->labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui->labelImage->setScaledContents(true);
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
	completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	le->setCompleter(completer);
}

void MedyaLib::on_toolSaveInfos_clicked()
{
	auto strMap = dbHelper->getFieldStrings();
	dbHelper->addData(strMap.find(DatabaseHelper::PERSONS).value(),
					  ui->lineEditWho->text());
	dbHelper->addData(strMap.find(DatabaseHelper::DATES).value(),
					  ui->lineEditTimes->text());
	dbHelper->addData(strMap.find(DatabaseHelper::TAGS).value(),
					  ui->lineEditTags->text());
	dbHelper->addData(strMap.find(DatabaseHelper::LOCATIONS).value(),
					  ui->lineEditLocat->text());
	dbHelper->addData(strMap.find(DatabaseHelper::PATHS).value(),
					  currentMediaInformation.first);
}
