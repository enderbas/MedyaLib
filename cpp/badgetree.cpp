#include "badgetree.h"
#include "ui_badgetree.h"
BadgeTree::BadgeTree(QWidget *parent) : QWidget(parent), ui(new Ui::BadgeTree)
{
	ui->setupUi(this);
	ui->treeWidget->setHeaderLabel("Badges");
	ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
	connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
			SLOT(updateSelectedItemList(QTreeWidgetItem *, int)));
}

BadgeTree::~BadgeTree()
{
	delete ui;
}

QTreeWidgetItem *BadgeTree::addItem(const QString &itemName)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
	item->setText(0, itemName);
	item->setCheckState(0, Qt::Unchecked);
	item->setData(0, 500, itemName);
	ui->treeWidget->addTopLevelItem(item);
	return item;
}

void BadgeTree::addSubItem(const QString &parent, const QString &itemName)
{
	auto parentItemsList = ui->treeWidget->findItems(parent, Qt::MatchExactly);
	if (!parentItemsList.size())
		return;
	auto parentItem = parentItemsList.at(0);
	if (!parentItem) {
		parentItem = addItem(parent);
	}
	QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
	item->setText(0, itemName);
	item->setCheckState(0, Qt::Unchecked);
}

void BadgeTree::addSubItems(const QString &parent, const QStringList &itemList)
{
	for (const auto &item : itemList) {
		addSubItem(parent, item);
	}
}

QStringList BadgeTree::getSelectedBadges(const Badges &badge)
{
	switch (badge) {
	case TAGS:
		return selectedTags;
	case LOCATIONS:
		return selectedLocations;
	case PERSONS:
		return selectedPersons;
	case TIMES:
		return selectedTimes;
	}
	return {};
}

void BadgeTree::updateSelectedItemList(QTreeWidgetItem *item, int column)
{
	auto parent = item->parent();
	if (!parent) {
		addCheckToChildren(item, item->checkState(0));
		return;
	}
	QString parentName = parent->data(0, 500).toString();
	if (item->checkState(0) == Qt::Checked) {
		addtoItemList(parentName, item->text(0));
	} else {
		removeFromItemList(parentName, item->text(0));
	}
}

void BadgeTree::addCheckToChildren(const QTreeWidgetItem *parent,
								   bool checkState)
{
	int childCount = parent->childCount();
	for (int i = 0; i < childCount; i++) {
		if (checkState)
			parent->child(i)->setCheckState(0, Qt::Checked);
		else
			parent->child(i)->setCheckState(0, Qt::Unchecked);
	}
}

void BadgeTree::addtoItemList(const QString &parentName, const QString &item)
{
	if (parentName == "locations")
		selectedLocations.push_back(item);
	if (parentName == "tags")
		selectedTags.push_back(item);
	if (parentName == "persons")
		selectedPersons.push_back(item);
	if (parentName == "dates")
		selectedTimes.push_back(item);
}

void BadgeTree::removeFromItemList(const QString &parentName,
								   const QString &item)
{
	if (parentName == "locations")
		selectedLocations.removeAll(item);
	if (parentName == "tags")
		selectedTags.removeAll(item);
	if (parentName == "persons")
		selectedPersons.removeAll(item);
	if (parentName == "dates")
		selectedTimes.removeAll(item);
}
