#include "badgetree.h"
#include "ui_badgetree.h"

BadgeTree::BadgeTree(QWidget *parent) : QWidget(parent), ui(new Ui::BadgeTree)
{
	ui->setupUi(this);
	ui->treeWidget->setHeaderLabel("Badges");
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

QMap<QString, QStringList> BadgeTree::getSelectedBadges()
{
	//selected degil checked olmali
	auto selectedItemsList = ui->treeWidget->selectedItems();
	QString parentText;
	QString text;
	QMap<QString, QStringList> resp {};
	for (const auto &item : qAsConst(selectedItemsList)) {
		if (!item->parent())
			continue;
		parentText = item->parent()->text(0);
		text = item->text(0);
		if (resp.contains(parentText))
			resp.find(parentText)->append(text);
		else
			resp.insert(parentText, {text});
	}
	return resp;
}
