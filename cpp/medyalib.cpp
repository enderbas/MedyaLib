#include "medyalib.h"
#include "./ui_medyalib.h"
#include <QCompleter>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
MedyaLib::MedyaLib(QWidget *parent) : QMainWindow(parent), ui(new Ui::MedyaLib)
{
	ui->setupUi(this);
	searchPres = new PresentationWidget(this);
	ui->layoutSplitter->addWidget(searchPres);
	galleryPres = new PresentationWidget(this);
	ui->layoutPresentation->addWidget(galleryPres);

	dbHelper = new DatabaseHelper(QDir::currentPath(), "medyalib.db");
	dbHelper->createDb();
	setCompleters();
	tree = new BadgeTree(this);
	ui->layoutTree->addWidget(tree);
	initBadgeTree();
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
	currentWorkPath = directory.path();
	QStringList medias = directory.entryList(QStringList() << "*.jpg"
														   << "*.JPG"
														   << "*.png"
														   << "*.PNG",
											 QDir::Files);
	searchPres->showListedItems(medias, directory);
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
	addCompleter(ui->lineDetailedWho, "persons");
	addCompleter(ui->lineDetailedTimes, "dates");
	addCompleter(ui->lineDetailedTags, "tags");
	addCompleter(ui->lineDetailedLocation, "locations");
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

void MedyaLib::initBadgeTree()
{
	auto strMap = dbHelper->getFieldStrings();
	for(const auto &field : strMap) {
		if (field == "paths")
			continue;
		tree->addItem(field);
		auto list = dbHelper->getColumnItems(field);
		if(list.size())
			tree->addSubItems(field, list);
	}
}

void MedyaLib::addToSearchBars(QLineEdit *line, const QStringList &badges)
{
	QString text = "";
	for (const auto &badge : badges) {
		text.append(badge + ";");
	}
	text.remove(text.size() - 1, 1);
	line->setText(text);
}

void MedyaLib::on_toolSaveInfos_clicked()
{
	auto strMap = dbHelper->getFieldStrings();
	QString path = searchPres->getPath();
	QString name = path.split('/').last();
	QString ext = name.split('.').last();
	QString id = "";
	QString mediaId;

	mediaId = dbHelper->addMediaData(name, ext);
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
	if (!currentWorkPath.isEmpty()) {
		id = dbHelper->addData(strMap.find(DatabaseHelper::PATHS).value(),
							   currentWorkPath);
		dbHelper->addMediaCombinedData("media_paths", id, mediaId);
	}
}

void MedyaLib::on_toolSearch_clicked()
{
}

void MedyaLib::on_pushApplyFilters_clicked()
{
	addToSearchBars(ui->lineDetailedTags, tree->getSelectedBadges(BadgeTree::TAGS));
	addToSearchBars(ui->lineDetailedLocation, tree->getSelectedBadges(BadgeTree::LOCATIONS));
	addToSearchBars(ui->lineDetailedWho, tree->getSelectedBadges(BadgeTree::PERSONS));
	addToSearchBars(ui->lineDetailedTimes, tree->getSelectedBadges(BadgeTree::TIMES));
}
