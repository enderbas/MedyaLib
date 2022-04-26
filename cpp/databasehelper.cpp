#include "databasehelper.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
DatabaseHelper::DatabaseHelper(const QString &databasePath, const QString &databaseName, QObject *parent)
	: dbName(databaseName), QObject {parent}
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	QString db_name_path = databasePath + "/" + databaseName;
	db.setDatabaseName(db_name_path);

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

void DatabaseHelper::createDb()
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
	if (tableName.isEmpty() || combined_id.isEmpty() || mediaId.isEmpty())
		return "";
	QString combColName = tableName.split("_").at(1) + "_id";
	QString queryText =
		"INSERT INTO %1 (media_id, %2) SELECT '%3', '%4' WHERE NOT EXISTS "
		"(SELECT * FROM %1 WHERE (%1.media_id = '%3' and %1.%2 = '%4'))";
	queryText = queryText.arg(tableName, combColName, mediaId, combined_id);
	QSqlQuery query;
	query.prepare(queryText);
	query.exec();
	printQueryError(query);
	return getDataFromTable("id", tableName, combColName, combined_id);
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

QStringList DatabaseHelper::search(const QMap<QString, QStringList> &queryMap)
{
	auto createTempTable = [](QString &query, const QString &tempName,
							  const QString &createdFrom) {
		QString tempTable = "DROP TABLE IF EXISTS %1; CREATE TEMP TABLE %1 AS "
							"SELECT * FROM %2 ";
		if (query.isEmpty())
			query = tempTable.arg(tempName, createdFrom);
		else
			query.append(tempTable.arg(tempName, createdFrom));
	};
	auto createDoubleInnerJoin = [](QString &query, const QString &table) {
		QString temp = "INNER JOIN media_%1 on medias.id = media_%1.media_id "
					   "INNER JOIN %1 on %1.id = media_%1.%1_id ";
		query.append(temp.arg(table));
	};
	auto createInnerJoin = [](QString &query, const QString &table,
							  QString createdfrom) {
		QString temp = "INNER JOIN %1 on %2.media_id = %1.media_id ";
		query.append(temp.arg(table, createdfrom));
	};

	QMapIterator<QString, QStringList> i(queryMap);
	QString query;
	createTempTable(query, "tt1", "medias");
	while (i.hasNext()) {
		i.next();
		if (!i.value().size())
			continue;
		createDoubleInnerJoin(query, i.key());
	}
	query.append(";");
	int e = 100;
	QMapIterator<QString, QStringList> j(queryMap);
	while (j.hasNext()) {
		j.next();
		if (!j.value().size())
			continue;
		for (const auto &text : j.value()) {
			QString ttname = "tt" + QString::number(e);
			createTempTable(query, ttname, "tt1");
			QString whereQuery = "WHERE tt1.%1 = '%2' ;";
			whereQuery =
				whereQuery.arg(createColumnNameFromTableName(j.key()), text);
			query.append(whereQuery);
			e++;
		}
	}
	e = 101;
	QMapIterator<QString, QStringList> k(queryMap);
	createTempTable(query, "tt2", "tt100");
	while (k.hasNext()) {
		k.next();
		QString ttname = "tt" + QString::number(e);
		createInnerJoin(query, ttname, "tt100");
		e++;
	}
	query.append(";");
	QString selectGroup = "SELECT %1.name FROM %1 GROUP BY %1.name;";
	query.append(selectGroup.arg("tt2"));
	QSqlQuery sqlquery;
	QStringList commaSeperated = query.split(";");
	for (const auto &c : qAsConst(commaSeperated)) {
		if (c.isEmpty())
			continue;
		if (!sqlquery.exec(c)) {
			qDebug()
				<< QString(
					   "One of the query failed to execute. Error detail: " +
					   sqlquery.lastError().text())
					   .toLocal8Bit();
		}
	}
	QStringList response;
	while (sqlquery.next()) {
		response << sqlquery.value(0).toString();
	}
	sqlquery.finish();
	printQueryError(sqlquery);
	return response;
}
