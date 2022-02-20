#include "databasehelper.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

DatabaseHelper::DatabaseHelper(const QString &databaseName, QObject *parent)
	: dbName(databaseName), QObject {parent}
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbName);

	fieldStrings.insert(PATHS, "paths");
	fieldStrings.insert(LOCATIONS, "locations");
	fieldStrings.insert(PERSONS, "persons");
	fieldStrings.insert(TAGS, "tags");
	fieldStrings.insert(DATES, "dates");

	if (!db.open()) {
		QMessageBox::critical(
			nullptr, QObject::tr("Cannot open database"),
			QObject::tr("Unable to establish a database connection.\n\n"
						"Click Cancel to exit."),
			QMessageBox::Cancel);
		return;
	}
}

void DatabaseHelper::createDb(const QString &name)
{
	createMediasDB();

	QSqlQuery query;
	for (const auto &field : qAsConst(fieldStrings)) {
		if (db.tables().contains(field))
			continue;
		// create single databases
		QString col = field;
		col.remove(col.size() - 1, 1);
		QString singleDBtext =
			"CREATE TABLE %1 (id INTEGER UNIQUE NOT NULL, %2 TEXT NOT "
			"NULL UNIQUE, PRIMARY KEY(id AUTOINCREMENT))";
		singleDBtext = singleDBtext.arg(field, col);
		query.exec(singleDBtext);
		printQueryError(query);
		// create combination databases
		QString combTableName = "media_" + field;					//%1
		QString combId = field + "_id";								//%2
		QString foreignKeyName = "fk_" + field + "_media_" + field; //%3
		QString ref = field;										//%4
		QString constraintName = "fk_medias_media_" + field;		//%5
		QString multiDBTtext =
			"CREATE TABLE %1 (id INTEGER NOT NULL UNIQUE, "
			"media_id INTEGER NOT NULL, %2 INTEGER NOT NULL,"
			"CONSTRAINT %3 FOREIGN KEY(%2) REFERENCES "
			"%4(id) ON UPDATE CASCADE ON DELETE CASCADE,"
			"CONSTRAINT %5 FOREIGN KEY(media_id) "
			"REFERENCES medias(id) ON UPDATE CASCADE ON DELETE CASCADE,"
			"PRIMARY KEY(id AUTOINCREMENT))";
		multiDBTtext = multiDBTtext.arg(combTableName, combId, foreignKeyName,
										ref, constraintName);
		query.exec(multiDBTtext);
		printQueryError(query);
	}
}

QString DatabaseHelper::addData(const QString &table, const QString &input)
{
	QString columnName = createColumnNameFromTableName(table);
	QString rawQuery("INSERT INTO %1 (%2) VALUES (:%2)");
	QString a_rawQuery = rawQuery.arg(table, columnName);
	QString bindField = ":" + columnName;
	QSqlQuery query;
	query.prepare(a_rawQuery);
	query.bindValue(bindField, input);
	query.exec();
	printQueryError(query);
	return getDataFromTable("id", table, columnName, input);
}

QString DatabaseHelper::addMediaData(const QString &name,
									 const QString &extension)
{
	QString queryText =
		"INSERT INTO medias (name, extension) VALUES(:name, :extension)";
	QSqlQuery query;
	query.prepare(queryText);
	query.bindValue(":name", name);
	query.bindValue(":extension", extension);
	query.exec();
	printQueryError(query);
	return getDataFromTable("id", "medias", "name", name);
}

QString DatabaseHelper::addMediaCombinedData(const QString &tableName,
											 const QString &combined_id,
											 const QString &mediaId)
{
	QString combColName = tableName.split("_").at(1) + "_id";
	QString queryText = "INSERT INTO %1 (media_id, %2) VALUES(:media_id, :%2)";
	queryText = queryText.arg(tableName, combColName);
	qDebug() << queryText;
	QSqlQuery query;
	query.prepare(queryText);
	query.bindValue(":media_id", mediaId);
	query.bindValue(":" + combColName, combined_id);
	query.exec();
	printQueryError(query);
	return getDataFromTable("id", tableName, combColName, combined_id);
}

bool DatabaseHelper::isExist(const QString &tableName, const QString &input)
{
	QString text = "SELECT %1 FROM %2 WHERE %1 = (:x)";
	QString colName = createColumnNameFromTableName(tableName);
	text = text.arg(colName, tableName);

	QSqlQuery query;
	query.prepare(text);
	query.bindValue(":x", input);

	if (query.exec()) {
		if (query.next()) {
			qDebug() << input << " exists in " << tableName;
			qDebug() << getDataFromTable("id", "persons", "person", "Engin");
			return true;
		}
	}
	return false;
}

QString DatabaseHelper::getDataFromTable(const QString &select,
										 const QString &from,
										 const QString &where,
										 const QString &variable)
{
	QSqlQuery query;
	QString text = "SELECT %1 FROM %2 WHERE %3 = (:x)";
	text = text.arg(select, from, where);
	query.prepare(text);
	query.bindValue(":x", variable);
	query.exec();
	QString data = "";
	while (query.next()) {
		data = query.value(0).toString();
	}
	return data;
}

void DatabaseHelper::createMediasDB()
{
	if (db.tables().contains("medias"))
		return;
	QSqlQuery query;
	query.exec(
		"CREATE TABLE medias (id INTEGER NOT NULL UNIQUE, name TEXT NOT "
		"NULL UNIQUE, extension TEXT NOT NULL,PRIMARY KEY(id AUTOINCREMENT))");
}

void DatabaseHelper::printQueryError(const QSqlQuery &query)
{
	if (query.lastError().isValid())
		qDebug() << "QUERY Error: " << query.lastError().text();
}

QString DatabaseHelper::createColumnNameFromTableName(const QString &tablename)
{
	QString tn = tablename;
	return tn.remove(tn.length() - 1, 1);
}

const QHash<DatabaseHelper::FIELDS, QString> &
DatabaseHelper::getFieldStrings() const
{
	return fieldStrings;
}

QStringList DatabaseHelper::getColumnItems(const QString &tableName)
{
	if (colItems.size())
		colItems.clear();
	QSqlQuery query;
	QString q = "SELECT %1 FROM %2";
	q = q.arg(createColumnNameFromTableName(tableName), tableName);
	query.exec(q);
	while (query.next()) {
		colItems.push_back(query.value(0).toString());
	}
	return colItems;
}
