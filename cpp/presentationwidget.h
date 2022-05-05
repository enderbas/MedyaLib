#ifndef PRESENTATIONWIDGET_H
#define PRESENTATIONWIDGET_H

#include "qscrollarea.h"
#include <QDir>
#include <QFuture>
#include <QLabel>
#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class PresentationWidget;
}

class PresentationWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationWidget(QWidget *parent = nullptr);
	~PresentationWidget();
	void showListedItems(const QStringList &list, const QDir &workPath);
	void showListedItems(const QStringList &list);
	const QString &getPath() const;
	void zoomIn();
	void zoomOut();

	double getScaleFactor() const;

private slots:
	void draw(QListWidgetItem *item);

private:
	enum MediaProperties {
		FULL_PATH = 15,
		NAME,
		SIZE,
		EXTENSION,
		MIME_TYPE,
		MODIFIED,
		ACCESSED
	};
	QString path;
	Ui::PresentationWidget *ui;
	QLabel *labelImage = nullptr;
	QListWidget *thumbnailNewImages = nullptr;
	QFuture<void> futureThr;
	QScrollArea *scrollArea = nullptr;
	double scaleFactor = 1;

protected:
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
};

#endif // PRESENTATIONWIDGET_H
