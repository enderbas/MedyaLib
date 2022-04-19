#ifndef PRESENTATIONWIDGET_H
#define PRESENTATIONWIDGET_H

#include <QWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QDir>
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

private slots:
	void draw(QListWidgetItem *item);

private:
	enum MediaProperties {
		FULL_PATH,
		NAME,
		SIZE,
		EXTENSION,
		MIME_TYPE,
		MODIFIED,
		ACCESSED
	};
	Ui::PresentationWidget *ui;
	QLabel *labelImage = nullptr;
	QListWidget *thumbnailNewImages = nullptr;
};

#endif // PRESENTATIONWIDGET_H
