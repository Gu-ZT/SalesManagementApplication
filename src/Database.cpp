#include "Database.h"

Database db = Database();

bool Database::isInited() {
    return db.inited;
}

void Database::init() {
    db.database = QSqlDatabase::addDatabase("QSQLITE");
    db.database.setDatabaseName(QString::fromStdString("sales.db"));
    if (!db.database.open()) {
        qDebug() << "Unable to open database!" << db.database.lastError().text();
        error(DATABASE_CONNECT_ERROR);
    } else {
        qDebug() << "Database: connection ok";
        db.inited = true;
    }
}

template<typename T>
int DataMapper<T>::getVersionById(long long int pId) {
    std::stringstream sb;
    sb << "SELECT `version` FROM `" << this->getTableName() << "` WHERE `id`=" << pId << " AND `del_flag`=0;";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
