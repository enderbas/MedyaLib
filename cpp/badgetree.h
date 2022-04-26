#ifndef BADGETREE_H
#define BADGETREE_H

#include <QWidget>
#include <QTreeWidgetItem>
namespace Ui {
class BadgeTree;
}

class BadgeTree : public QWidget
{
	Q_OBJECT
	
public:
	explicit BadgeTree(QWidget *parent = nullptr);
	~BadgeTree();
	QTreeWidgetItem *addItem(const QString &itemName);
	void addSubItem(const QString &parent, const QString &itemName);
	void addSubItems(const QString &parent, const QStringList &itemList);
	QMap<QString, QStringList> getSelectedBadges();
private:
	Ui::BadgeTree *ui;
};

#endif // BADGETREE_H
