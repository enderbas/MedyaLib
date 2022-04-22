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

	struct queryStruct {
		QString columnName;
		QString data;
	};

	DatabaseHelper(const QString &databasePath, const QString &databaseName, QObject *parent = nullptr);
	void createDb();
	QString addData(const QString &table, const QString &input);
	QString addMediaData(const QString &name, const QString &extension);
	QString addMediaCombinedData(const QString &tableName,
								 const QString &combined_id,
								 const QString &mediaId);
	QString getDataFromTable(const QString &select, const QString &from,
							 const QString &where, const QString &variable);

	const QHash<FIELDS, QString> &getFieldStrings() const;
	QStringList getColumnItems(const QString &tableName);

private:
	void createMediasDB();
	void printQueryError(const QSqlQuery &query);
	QString createColumnNameFromTableName(const QString &tablename);

private:
	QSqlDatabase db;
	QString dbName;
	QHash<FIELDS, QString> fieldStrings;
	QStringList colItems;
};

#endif // DATABASEHELPER_H
