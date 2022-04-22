#ifndef PRESENTATIONWIDGET_H
#define PRESENTATIONWIDGET_H

#include <QWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QDir>
#include <QFuture>

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
	
	const QString &getPath() const;
	
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
};

#endif // PRESENTATIONWIDGET_H
