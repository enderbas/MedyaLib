#ifndef BADGETREE_H
#define BADGETREE_H

#include <QTreeWidgetItem>
#include <QWidget>
namespace Ui
{
class BadgeTree;
}

class BadgeTree : public QWidget
{
	Q_OBJECT

public:
	explicit BadgeTree(QWidget *parent = nullptr);
	~BadgeTree();
	enum Badges { TAGS, LOCATIONS, PERSONS, TIMES };

	QTreeWidgetItem *addItem(const QString &itemName);
	void addSubItem(const QString &parent, const QString &itemName);
	void addSubItems(const QString &parent, const QStringList &itemList);
	QStringList getSelectedBadges(const Badges &badge);
	void clear();

public slots:
	void updateSelectedItemList(QTreeWidgetItem *item, int column);

private:
	void addCheckToChildren(const QTreeWidgetItem *parent, bool checkState);
	void addtoItemList(const QString &parentName, const QString &item);
	void removeFromItemList(const QString &parentName, const QString &item);

private:
	Ui::BadgeTree *ui;
	QStringList selectedTags;
	QStringList selectedLocations;
	QStringList selectedPersons;
	QStringList selectedTimes;
};

#endif // BADGETREE_H
