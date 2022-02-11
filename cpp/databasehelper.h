#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QHash>
#include <QObject>
#include <QSqlDatabase>
#include <QString>

class DatabaseHelper : public QObject
{
	Q_OBJECT

public:
	enum FIELDS { PATHS, LOCATIONS, PERSONS, TAGS, DATES };

	DatabaseHelper(const QString &databaseName, QObject *parent = nullptr);
	void createDb(const QString &name);
	void addData(const QString &table, const QString &input);
	bool isExist(const QString &db, const QString &input);
	QString getDataFromTable(const QString &select, const QString &from,
							 const QString &where, const QString &variable);

	const QHash<FIELDS, QString> &getFieldStrings() const;

private:
	void createMediasDB();
	void printQueryError(const QSqlQuery &query);
	QString createColumnNameFromTableName(const QString &tablename);

private:
	QSqlDatabase db;
	QString dbName;
	QHash<FIELDS, QString> fieldStrings;
};

#endif // DATABASEHELPER_H
