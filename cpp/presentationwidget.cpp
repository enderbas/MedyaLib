#include "presentationwidget.h"
#include "ui_presentationwidget.h"
#include <QHBoxLayout>
#include <QImageReader>
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

	thumbnailNewImages->setViewMode(QListWidget::IconMode);
	thumbnailNewImages->setIconSize(QSize(200, 150));
	thumbnailNewImages->setResizeMode(QListWidget::Adjust);

	QSplitter *splitter = new QSplitter(Qt::Horizontal);
	splitter->addWidget(thumbnailNewImages);
	splitter->addWidget(labelImage);
	QHBoxLayout *lay = new QHBoxLayout(this);
	lay->addWidget(splitter);

	connect(thumbnailNewImages, SIGNAL(itemClicked(QListWidgetItem*)), this,
			SLOT(draw(QListWidgetItem*)));
}
// https://stackoverflow.com/questions/26829754/how-to-use-the-threads-to-create-the-images-thumbnail
// TODO: draw images using threads

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
	labelImage->clear();
	labelImage->setPixmap(QPixmap::fromImage(newImage));
}

const QString &PresentationWidget::getPath() const
{
	return path;
}
