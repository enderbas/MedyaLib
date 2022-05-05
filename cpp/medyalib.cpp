#include "medyalib.h"
#include "./ui_medyalib.h"
#include <QCompleter>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
MedyaLib::MedyaLib(QWidget *parent) : QMainWindow(parent), ui(new Ui::MedyaLib)
{
	ui->setupUi(this);
	createActions();
	ui->toolSearch->setIcon(QIcon(":icons/search_black_24dp.svg"));
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
	ui->checkExactMatch->setVisible(false);
}

MedyaLib::~MedyaLib()
{
	delete ui;
}

void MedyaLib::open()
{
	ui->stackedWidget->setCurrentIndex(NEWMEDIA);
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

void MedyaLib::gallery()
{
	ui->stackedWidget->setCurrentIndex(GALLERY);
	initBadgeTree();
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
	tree->clear();
	auto strMap = dbHelper->getFieldStrings();
	for (const auto &field : strMap) {
		if (field == "paths")
			continue;
		tree->addItem(field);
		auto list = dbHelper->getColumnItems(field);
		if (list.size())
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

void MedyaLib::createActions()
{
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QAction *openAct =
		fileMenu->addAction(tr("&Open..."), this, &MedyaLib::open);
	openAct->setShortcut(QKeySequence::Open);

	QAction *galleryAct =
		fileMenu->addAction(tr("&Gallery..."), this, &MedyaLib::gallery);
	galleryAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

	zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, [this]() {
		if (ui->stackedWidget->currentIndex() == GALLERY) {
			if (galleryPres->getScaleFactor() < 3.0)
				galleryPres->zoomIn();
		} else if (ui->stackedWidget->currentIndex() == NEWMEDIA) {
			if (searchPres->getScaleFactor() < 3.0)
				searchPres->zoomIn();
		}
	});
	zoomInAct->setShortcut(QKeySequence::ZoomIn);

	zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, [this]() {
		if (ui->stackedWidget->currentIndex() == GALLERY) {
			if (galleryPres->getScaleFactor() > 0.333)
				galleryPres->zoomOut();
		} else if (ui->stackedWidget->currentIndex() == NEWMEDIA) {
			if (searchPres->getScaleFactor() > 0.333)
				searchPres->zoomOut();
		}
	});

	zoomOutAct->setShortcut(QKeySequence::ZoomOut);
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
	QMap<QString, QStringList> queryMap;
	if (!ui->lineDetailedWho->text().isEmpty()) {
		auto l = ui->lineDetailedWho->text().split(";");
		queryMap.insert("persons",l);
	}
	if (!ui->lineDetailedLocation->text().isEmpty()) {
		auto l = ui->lineDetailedLocation->text().split(";");
		queryMap.insert("locations",l);
	}
	if (!ui->lineDetailedTags->text().isEmpty()) {
		auto l = ui->lineDetailedTags->text().split(";");
		queryMap.insert("tags",l);
	}
	if (!ui->lineDetailedTimes->text().isEmpty()) {
		auto l = ui->lineDetailedTimes->text().split(";");
		queryMap.insert("dates",l);
	}
	auto resp = dbHelper->search(queryMap);
	galleryPres->showListedItems(resp);
}

void MedyaLib::on_pushApplyFilters_clicked()
{
	addToSearchBars(ui->lineDetailedTags, tree->getSelectedBadges(BadgeTree::TAGS));
	addToSearchBars(ui->lineDetailedLocation, tree->getSelectedBadges(BadgeTree::LOCATIONS));
	addToSearchBars(ui->lineDetailedWho, tree->getSelectedBadges(BadgeTree::PERSONS));
	addToSearchBars(ui->lineDetailedTimes, tree->getSelectedBadges(BadgeTree::TIMES));
}
