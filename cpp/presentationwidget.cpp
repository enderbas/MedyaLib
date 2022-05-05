#include "presentationwidget.h"
#include "ui_presentationwidget.h"
#include <QAction>
#include <QHBoxLayout>
#include <QImageReader>
#include <QScrollBar>
#include <QSplitter>
#include <QtConcurrent>

PresentationWidget::PresentationWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::PresentationWidget)
{
	ui->setupUi(this);
	thumbnailNewImages = new QListWidget(this);
	labelImage = new QLabel(this);
	labelImage->setBackgroundRole(QPalette::Base);
	labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	labelImage->setScaledContents(true);

	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(labelImage);
	scrollArea->setVisible(false);

	thumbnailNewImages->setViewMode(QListWidget::IconMode);
	thumbnailNewImages->setIconSize(QSize(200, 150));
	thumbnailNewImages->setResizeMode(QListWidget::Adjust);

	QSplitter *splitter = new QSplitter(Qt::Horizontal);
	splitter->addWidget(thumbnailNewImages);
	splitter->addWidget(scrollArea);
	splitter->setSizes(
		{splitter->size().width() * 2 / 3, splitter->size().width() / 3});
	QHBoxLayout *lay = new QHBoxLayout(this);
	lay->addWidget(splitter);

	connect(thumbnailNewImages, SIGNAL(itemClicked(QListWidgetItem*)), this,
			SLOT(draw(QListWidgetItem*)));
}

PresentationWidget::~PresentationWidget()
{
	delete ui;
}

void PresentationWidget::showListedItems(const QStringList &list,
										 const QDir &workPath)
{
	auto listItems = [=]() {
		for (const auto &val : qAsConst(list)) {
			QString path = workPath.absoluteFilePath(val);
			QListWidgetItem *item = new QListWidgetItem(QIcon(path), val);
			item->setData(MediaProperties::FULL_PATH, path);
			thumbnailNewImages->addItem(item);
		}
	};
	thumbnailNewImages->clear();
	futureThr.waitForFinished();
	futureThr = QtConcurrent::run(listItems);
}

void PresentationWidget::showListedItems(const QStringList &list)
{
	auto listItems = [=]() {
		for (const auto &val : qAsConst(list)) {
			QListWidgetItem *item = new QListWidgetItem(QIcon(val), "");
			item->setData(MediaProperties::FULL_PATH, val);
			thumbnailNewImages->addItem(item);
		}
	};
	thumbnailNewImages->clear();
	futureThr.waitForFinished();
	futureThr = QtConcurrent::run(listItems);
}

void PresentationWidget::draw(QListWidgetItem *item)
{
	path = item->data(MediaProperties::FULL_PATH).toString();
	QImageReader reader(path);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	if (newImage.isNull()) {
		return;
	}
	setWindowFilePath(path);
	labelImage->clear();
	labelImage->setPixmap(QPixmap::fromImage(newImage));

	scaleFactor = 1.0;
	scrollArea->setVisible(true);
	labelImage->adjustSize();
}

double PresentationWidget::getScaleFactor() const
{
	return scaleFactor;
}

void PresentationWidget::scaleImage(double factor)
{
	scaleFactor *= factor;
	labelImage->resize(scaleFactor *
					   labelImage->pixmap()->size());

	adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(scrollArea->verticalScrollBar(), factor);
}

void PresentationWidget::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value() +
							((factor - 1) * scrollBar->pageStep() / 2)));
}

const QString &PresentationWidget::getPath() const
{
	return path;
}

void PresentationWidget::zoomIn()
{
	scaleImage(1.25);
}

void PresentationWidget::zoomOut()
{
	scaleImage(0.8);
}
