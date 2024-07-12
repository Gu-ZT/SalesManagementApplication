#include <utility>

#include "DataEntity.h"

DataEntity::DataEntity() : id(SnowflakeIDGenerator::id()), createTime(Time::current()), updateTime(Time::current()) {}

long long DataEntity::getId() const {
    return this->id;
}

int DataEntity::getVersion() const {
    return this->version;
}

long long DataEntity::getCreateTime() const {
    return this->createTime;
}

long long DataEntity::getUpdateTime() const {
    return this->updateTime;
}

bool DataEntity::isDelete() const {
    return this->delFlag;
}

DataEntity &DataEntity::setId(long long pId) {
    this->id = pId;
    return *this;
}

DataEntity &DataEntity::setVersion(int pVersion) {
    this->version = pVersion;
    return *this;
}

DataEntity &DataEntity::setCreateTime(long long pCreateTime) {
    this->createTime = pCreateTime;
    return *this;
}

DataEntity &DataEntity::setUpdateTime(long long pUpdateTime) {
    this->updateTime = pUpdateTime;
    return *this;
}

DataEntity &DataEntity::setDelete(bool pDelFlag) {
    this->delFlag = pDelFlag;
    return *this;
}

InventoryType::InventoryType(std::string pName) : DataEntity(), name(std::move(pName)) {}

std::string InventoryType::getName() const {
    return this->name;
}

InventoryType &InventoryType::setName(std::string &pName) {
    this->name = pName;
    return *this;
}

bool InventoryType::isEmpty() {
    return this == &InventoryTypeEmpty;
}

const char *InventoryTypeMapper::getTableName() {
    return "type";
}

bool InventoryTypeMapper::createTable() {
    QString qString = "create table `type`\n"
                      "(\n"
                      "    `id`          INTEGER           not null\n"   // 标识符
                      "        constraint inventory_pk primary key,\n"
                      "    `version`     INTEGER default 0 not null,\n"  // 乐观锁
                      "    `create_time` INTEGER           not null,\n"  // 创建时间
                      "    `update_time` INTEGER           not null,\n"  // 更新时间
                      "    `del_flag`    INTEGER default 0 not null,\n"  // 逻辑删除
                      "    `name`        TEXT              not null\n"   // 种类名称
                      ");";
    QSqlQuery query;
    query.prepare(qString);
    if (!query.exec()) {
        qDebug() << "Error create type table\n";
        return false;
    }
    return true;
}

QStringList InventoryTypeMapper::getType() {
    QStringList items = {};
    QSqlQuery query;
    query.prepare(QString::fromStdString("SELECT * FROM ") + this->getTableName());
    if (query.exec()) {
        while (query.next()) {
            QString type = query.value(5).toString();
            items.append(type);
        }
        return items;
    }
    return items;
}

//通过种类名称查询结果
InventoryType InventoryTypeMapper::getByType(const std::string &pType) {
    QSqlQuery query;
    query.prepare(QString::fromStdString("SELECT * FROM ") + this->getTableName() + " WHERE name = ?;");
    query.bindValue(0, QString::fromStdString(pType));
    if (query.exec() && query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        QString name = query.value(5).toString();

        std::unique_ptr<InventoryType> inventoryType = std::make_unique<InventoryType>(name.toStdString());

        inventoryType->setId(id);
        inventoryType->setVersion(version);
        inventoryType->setCreateTime(createTime);
        inventoryType->setUpdateTime(updateTime);
        inventoryType->setDelete(delFlag);
        return *inventoryType;
    }
    return InventoryTypeEmpty;
}

QList<InventoryType> InventoryTypeMapper::getList(int maxSize) {
    QList<InventoryType> list = {};
    if (!Database::isInited())return list;
    if (Database::isInited()) {
        QSqlQuery query;
        query.prepare(
                QString::fromStdString("SELECT * FROM ")
                + this->getTableName()
                + " WHERE del_flag = 0 ORDER BY `id` DESC LIMIT :maxSize;"
        );
        query.bindValue(":maxSize", maxSize);
        if (query.exec()) {
            while (query.next() && list.size() < maxSize) {
                long long id = query.value(0).toLongLong();
                int version = query.value(1).toInt();
                long long createTime = query.value(2).toLongLong();
                long long updateTime = query.value(3).toLongLong();
                bool delFlag = query.value(4).toBool();
                QString name = query.value(5).toString();
                std::unique_ptr<InventoryType> inventoryType = std::make_unique<InventoryType>(name.toStdString());
                inventoryType->setId(id);
                inventoryType->setVersion(version);
                inventoryType->setCreateTime(createTime);
                inventoryType->setUpdateTime(updateTime);
                inventoryType->setDelete(delFlag);
                list.append(*inventoryType);
            }
        }
    }
    return list;
}

InventoryType InventoryTypeMapper::getById(long long int pId) {
    std::stringstream sb;
    sb << "SELECT * FROM `" << this->getTableName() << "` WHERE `id`=" << pId << " AND `del_flag`=0;";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        QString name = query.value(5).toString();
        std::unique_ptr<InventoryType> inventoryType = std::make_unique<InventoryType>(name.toStdString());
        inventoryType->setId(id);
        inventoryType->setVersion(version);
        inventoryType->setCreateTime(createTime);
        inventoryType->setUpdateTime(updateTime);
        inventoryType->setDelete(delFlag);
        return *inventoryType;
    }
    return InventoryTypeEmpty;
}

DataMapper<InventoryType> &InventoryTypeMapper::save(InventoryType &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("INSERT INTO `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "`(`id`, `version`, `create_time`, `update_time`, `del_flag`, `name`) VALUES (:id, :version, :create_time, :update_time, :del_flag, :name);"
            )
    );

    query.bindValue(":id", data.getId());
    query.bindValue(":version", data.getVersion());
    query.bindValue(":create_time", Time::current());
    query.bindValue(":update_time", Time::current());
    query.bindValue(":del_flag", data.isDelete());
    query.bindValue(":name", QString::fromStdString(data.getName()));

    if (!query.exec()) {
        qDebug() << "Error inserting into type table\n";
    }

    return *this;
}

DataMapper<InventoryType> &InventoryTypeMapper::update(InventoryType &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "` SET `version`=:new_version, `update_time`=:update_time, `name`=:name WHERE id=:id AND `del_flag`=0 AND `version`=:version;"
            )
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        InventoryType inventoryType = this->getById(data.getId());
        if (inventoryType.isEmpty()) {
            qDebug() << "Error update type table, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", inventoryType.getId());
        query.bindValue(":version", inventoryType.getVersion());
        query.bindValue(":new_version", inventoryType.getVersion() + 1);
        query.bindValue(":update_time", Time::current());

        query.bindValue(":name", QString::fromStdString(data.getName()));

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error update type table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

DataMapper<InventoryType> &InventoryTypeMapper::del(InventoryType &data) {
    if (!Database::isInited())return *this;

    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "` SET `version`=:new_version, `update_time`=:update_time, `del_flag`=:del_flag WHERE `id`=:id AND `del_flag`=0 AND `version`=:version;"
            )
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        InventoryType inventoryType = this->getById(data.getId());

        if (inventoryType.isEmpty()) {
            qDebug() << "Error delete type table tuple, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", inventoryType.getId());
        query.bindValue(":version", inventoryType.getVersion());
        query.bindValue(":new_version", inventoryType.getVersion() + 1);
        query.bindValue(":update_time", Time::current());
        query.bindValue(":del_flag", true);

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error delete type table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

Inventory::Inventory(std::string pName, std::string pSpec)
        : DataEntity(), name(std::move(pName)), spec(std::move(pSpec)) {}

long long Inventory::getType() const {
    return this->type;
}

std::string Inventory::getName() const {
    return this->name;
}

std::string Inventory::getSpec() const {
    return this->spec;
}

Inventory &Inventory::setType(long long int &pType) {
    this->type = pType;
    return *this;
}

Inventory &Inventory::setName(std::string &pName) {
    this->name = std::move(pName);
    return *this;
}

Inventory &Inventory::setSpec(std::string &pSpec) {
    this->spec = std::move(pSpec);
    return *this;
}

bool Inventory::isEmpty() {
    return this == &InventoryEmpty;
}

const char *InventoryMapper::getTableName() {
    return "inventory";
}

bool InventoryMapper::createTable() {
    QString qString = "create table `inventory`\n"
                      "(\n"
                      "    `id`          INTEGER           not null\n"   // 标识符
                      "        constraint inventory_pk primary key,\n"
                      "    `version`     INTEGER default 0 not null,\n"  // 乐观锁
                      "    `create_time` INTEGER           not null,\n"  // 创建时间
                      "    `update_time` INTEGER           not null,\n"  // 更新时间
                      "    `del_flag`    INTEGER default 0 not null,\n"  // 逻辑删除
                      "    `type`        INTEGER           null,\n"      // 产品种类
                      "    `name`        TEXT              not null,\n"  // 产品名称
                      "    `spec`        TEXT              not null\n"   // 产品单位
                      ");";
    QSqlQuery query;
    query.prepare(qString);
    if (!query.exec()) {
        qDebug() << "Error create inventory table\n";
        return false;
    }
    return true;
}

//根据产品类别查找产品信息
QList<Inventory> InventoryMapper::getByType(long long pType) {
    QList<Inventory> List = {};
    QSqlQuery query;
    query.prepare(QString::fromStdString("SELECT * FROM ") + this->getTableName() + " WHERE type = ?");
    query.bindValue(0, pType);
    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        // 处理执行失败的情况，例如日志记录或错误处理
    } else {
        // 处理执行成功的情况，例如读取结果集
        while (query.next()) {
            long long id = query.value(0).toLongLong();
            int version = query.value(1).toInt();
            long long createTime = query.value(2).toLongLong();
            long long updateTime = query.value(3).toLongLong();
            bool delFlag = query.value(4).toBool();
            long long type = query.value(5).toLongLong();
            QString name = query.value(6).toString();
            QString spec = query.value(7).toString();
            std::unique_ptr<Inventory> inventory = std::make_unique<Inventory>(name.toStdString(), spec.toStdString());
            inventory->setId(id);
            inventory->setVersion(version);
            inventory->setCreateTime(createTime);
            inventory->setUpdateTime(updateTime);
            inventory->setDelete(delFlag);
            inventory->setType(type);
            List.append(*inventory);
        }
        return List;
    }
    return List;//没有查询到则返回一个空的表
}

Inventory InventoryMapper::getByName(const QString &pName) {
    QSqlQuery query;
    query.prepare(QString::fromStdString("SELECT * FROM ") + this->getTableName() + " WHERE pName = ?");
    query.bindValue(0, pName);
    while (query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        long long type = query.value(5).toLongLong();
        QString name = query.value(6).toString();
        QString spec = query.value(7).toString();
        std::unique_ptr<Inventory> inventory = std::make_unique<Inventory>(name.toStdString(), spec.toStdString());
        inventory->setId(id);
        inventory->setVersion(version);
        inventory->setCreateTime(createTime);
        inventory->setUpdateTime(updateTime);
        inventory->setDelete(delFlag);
        inventory->setType(type);
        return *inventory;
    }
    return InventoryEmpty;
}

Inventory InventoryMapper::getById(long long pId) {
    std::stringstream sb;
    sb << "SELECT * FROM `" << this->getTableName() << "` WHERE `id`=" << pId << " AND `del_flag`=0;";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        long long type = query.value(5).toLongLong();
        QString name = query.value(6).toString();
        QString spec = query.value(7).toString();
        std::unique_ptr<Inventory> inventory = std::make_unique<Inventory>(name.toStdString(), spec.toStdString());
        inventory->setId(id);
        inventory->setVersion(version);
        inventory->setCreateTime(createTime);
        inventory->setUpdateTime(updateTime);
        inventory->setDelete(delFlag);
        inventory->setType(type);
        return *inventory;
    }
    return InventoryEmpty;
}

QList<Inventory> InventoryMapper::getList(int maxSize) {
    QList<Inventory> list = {};
    std::stringstream sb;
    sb << "SELECT * FROM `"
       << this->getTableName()
       << "` WHERE `del_flag`=0 ORDER BY `id` DESC LIMIT "
       << maxSize
       << ";";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next() && list.size() < maxSize) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        long long type = query.value(5).toLongLong();
        QString name = query.value(6).toString();
        QString spec = query.value(7).toString();
        std::unique_ptr<Inventory> inventory = std::make_unique<Inventory>(name.toStdString(), spec.toStdString());
        inventory->setId(id);
        inventory->setVersion(version);
        inventory->setCreateTime(createTime);
        inventory->setUpdateTime(updateTime);
        inventory->setDelete(delFlag);
        inventory->setType(type);
        list.append(*inventory);
    }
    return list;
}

InventoryMapper &InventoryMapper::save(Inventory &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("INSERT INTO `") + this->getTableName() +
            "`(`id`, `version`, `create_time`, `update_time`, `del_flag`, `type`, `name`, `spec`) VALUES (:id, :version, :create_time, :update_time, :del_flag, :type, :name, :spec);"
    );

    query.bindValue(":id", data.getId());
    query.bindValue(":version", data.getVersion());
    query.bindValue(":create_time", Time::current());
    query.bindValue(":update_time", Time::current());
    query.bindValue(":del_flag", data.isDelete());
    query.bindValue(":type", data.getType());
    query.bindValue(":name", QString::fromStdString(data.getName()));
    query.bindValue(":spec", QString::fromStdString(data.getSpec()));

    if (!query.exec()) {
        qDebug() << "Error inserting into inventory table\n";
    }

    return *this;
}

InventoryMapper &InventoryMapper::update(Inventory &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `") + this->getTableName() +
            "` SET `version`=:new_version, `update_time`=:update_time, `type`=:type, `name`=:name, `spec`=:spec WHERE id=:id AND `del_flag`=0 AND `version`=:version;"
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        Inventory inventory = this->getById(data.getId());
        if (inventory.isEmpty()) {
            qDebug() << "Error update inventory table, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", inventory.getId());
        query.bindValue(":version", inventory.getVersion());
        query.bindValue(":new_version", inventory.getVersion() + 1);
        query.bindValue(":update_time", Time::current());

        query.bindValue(":type", data.getType());
        query.bindValue(":name", QString::fromStdString(data.getName()));
        query.bindValue(":spec", QString::fromStdString(data.getSpec()));

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error update inventory table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

InventoryMapper &InventoryMapper::del(Inventory &data) {
    if (!Database::isInited())return *this;

    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + this->getTableName()
            +
            "` SET `version`=:new_version, `update_time`=:update_time, `del_flag`=:del_flag WHERE `id`=:id AND `del_flag`=0 AND `version`=:version;"
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        Inventory inventory = this->getById(data.getId());

        if (inventory.isEmpty()) {
            qDebug() << "Error delete inventory table tuple, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", inventory.getId());
        query.bindValue(":version", inventory.getVersion());
        query.bindValue(":new_version", inventory.getVersion() + 1);
        query.bindValue(":update_time", Time::current());
        query.bindValue(":del_flag", true);

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error delete inventory table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

QList<Inventory> InventoryMapper::search(const std::string &pName, int maxSize) {
    QList<Inventory> list = {};
    if (!Database::isInited())return list;
    std::stringstream sb;
    sb << "SELECT * FROM `"
       << this->getTableName()
       << "` WHERE `del_flag`=0 AND `name` LIKE '%"
       << pName
       << "%' ORDER BY `id` DESC LIMIT "
       << maxSize
       << ";";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next() && list.size() < maxSize) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        long long type = query.value(5).toLongLong();
        std::string name = query.value(6).toString().toStdString();
        std::string spec = query.value(7).toString().toStdString();
        std::unique_ptr<Inventory> inventory = std::make_unique<Inventory>(name, spec);
        inventory->setId(id);
        inventory->setVersion(version);
        inventory->setCreateTime(createTime);
        inventory->setUpdateTime(updateTime);
        inventory->setDelete(delFlag);
        inventory->setType(type);
        list.append(*inventory);
    }
    return list;
}

//数据库表inventory_item
InventoryItem::InventoryItem(long long pInventory, std::string pCode, long long pManufacture, long long pShelfLife,
                             long long pStorageTime, int pIncomingAmount, int pSoldAmount,
                             int pStorageAmount, int pPurchase, int pPrice)
        : DataEntity(), inventory(pInventory), code(std::move(pCode)), manufacture(pManufacture), shelfLife(pShelfLife),
          storageTime(pStorageTime), incomingAmount(pIncomingAmount), soldAmount(pSoldAmount),
          storageAmount(pStorageAmount), purchase(pPurchase), price(pPrice) {}


long long InventoryItem::getInventory() const {
    return this->inventory;
}

std::string InventoryItem::getCode() const {
    return this->code;
}

long long InventoryItem::getManufacture() const {
    return this->manufacture;
}

long long InventoryItem::getShelfLife() const {
    return this->shelfLife;
}

long long InventoryItem::getStorageTime() const {
    return this->storageTime;
}

int InventoryItem::getIncomingAmount() const {
    return this->incomingAmount;
}

int InventoryItem::getSoldAmount() const {
    return this->soldAmount;
}

int InventoryItem::getStorageAmount() const {
    return this->storageAmount;
}

int InventoryItem::getPrice() const {
    return this->price;
}

InventoryItem &InventoryItem::setInventory(long long &pInventory) {
    this->inventory = pInventory;
    return *this;
}

InventoryItem &InventoryItem::setCode(const std::string &pCode) {
    this->code = pCode;
    return *this;
}

InventoryItem &InventoryItem::setManufacture(long long &pManufacture) {
    this->manufacture = pManufacture;
    return *this;
}

InventoryItem &InventoryItem::setShelfLife(long long &pShelfLife) {
    this->shelfLife = pShelfLife;
    return *this;
}

InventoryItem &InventoryItem::setStorageTime(long long &pStorageTime) {
    this->storageTime = pStorageTime;
    return *this;
}

InventoryItem &InventoryItem::setIncomingAmount(int &pIncomingAmount) {
    this->incomingAmount = pIncomingAmount;
    return *this;
}

InventoryItem &InventoryItem::setSoldAmount(int &pSoldAmount) {
    this->soldAmount = pSoldAmount;
    return *this;
}

InventoryItem &InventoryItem::setStorageAmount(int &pStorageAmount) {
    this->storageAmount = pStorageAmount;
    return *this;
}

InventoryItem &InventoryItem::setPrice(int &pPrice) {
    this->price = pPrice;
    return *this;
}

bool InventoryItem::isEmpty() {
    return this == &InventoryItemEmpty;
}

int InventoryItem::getPurchase() const {
    return this->purchase;
}

InventoryItem &InventoryItem::setPurchase(int &pPurchase) {
    this->purchase = pPurchase;
    return *this;
}

const char *InventoryItemMapper::getTableName() {
    return "inventory_item";
}

bool InventoryItemMapper::createTable() {
    QString qString = "create table `inventory_item`\n"
                      "(\n"
                      "    `id`              INTEGER           not null\n"  // 标识符
                      "        constraint inventory_item_pk primary key,\n"
                      "    `version`         INTEGER default 0 not null,\n" // 乐观锁
                      "    `create_time`     INTEGER           not null,\n" // 创建时间
                      "    `update_time`     INTEGER           not null,\n" // 更新时间
                      "    `del_flag`        INTEGER default 0 not null,\n" // 逻辑删除
                      "    `inventory`       INTEGER           not null,\n" // 库存标识符
                      "    `code`            TEXT              not null,\n" // 库存条码
                      "    `manufacture`     INTEGER           not null,\n" // 生产日期
                      "    `shelf_life`      INTEGER           not null,\n" // 保质期至
                      "    `storage_time`    INTEGER           not null,\n" // 入库日期
                      "    `incoming_amount` INTEGER           not null,\n" // 入库数量
                      "    `sold_amount`     INTEGER default 0 not null,\n" // 售出数量
                      "    `storage_amount`  INTEGER           not null,\n" // 库存数量
                      "    `purchase`        INTEGER           not null,\n" // 进价
                      "    `price`           INTEGER           not null\n"  // 价格
                      ");";
    QSqlQuery query;
    query.prepare(qString);
    if (!query.exec()) {
        qDebug() << "Error create inventory_item table\n";
        return false;
    }
    return true;
}

InventoryItem InventoryItemMapper::getByInventory(long long pInventory) {
    QSqlQuery query;
    query.prepare(QString::fromStdString("SELECT * FROM ") + this->getTableName() + " WHERE inventory = ?");
    query.bindValue(0, pInventory);  // 使用 % 符号来实现模糊查询

    if (query.exec()) {
        while (query.next()) {
            long long id = query.value(0).toLongLong();
            int version = query.value(1).toInt();
            long long createTime = query.value(2).toLongLong();
            long long updateTime = query.value(3).toLongLong();
            bool delFlag = query.value(4).toBool();
            long long inventory = query.value(5).toLongLong();
            QString code = query.value(6).toString();
            long long manufacture = query.value(7).toLongLong();
            long long shelf_life = query.value(8).toLongLong();
            long long storage_time = query.value(9).toLongLong();
            int incoming_amount = query.value(10).toInt();
            int sold_amount = query.value(11).toInt();
            int storage_amount = query.value(12).toInt();
            int purchase = query.value(13).toInt();
            int price = query.value(14).toInt();

            InventoryItem item(
                    inventory, code.toStdString(), manufacture, shelf_life, storage_time,
                    incoming_amount, sold_amount, storage_amount, purchase, price
            );

            item.setId(id);
            item.setVersion(version);
            item.setCreateTime(createTime);
            item.setUpdateTime(updateTime);
            item.setDelete(delFlag);

            return item;
        }
    }

    return InventoryItemEmpty;
}

QList<InventoryItem> InventoryItemMapper::getByDate(const QString &startDate, const QString &endDate) {
    QList<InventoryItem> result = {};
    QSqlQuery query;
    query.prepare(QString::fromStdString("SELECT * FROM ") + this->getTableName() +
                  " WHERE entry_date >=? AND entry_date <=?;");
    query.bindValue(0, startDate);
    query.bindValue(1, endDate);

    if (query.exec()) {
        while (query.next()) {
            long long id = query.value(0).toLongLong();
            int version = query.value(1).toInt();
            long long createTime = query.value(2).toLongLong();
            long long updateTime = query.value(3).toLongLong();
            bool delFlag = query.value(4).toBool();
            long long inventory = query.value(5).toLongLong();
            QString code = query.value(6).toString();
            long long manufacture = query.value(7).toLongLong();
            long long shelf_life = query.value(8).toLongLong();
            long long storage_time = query.value(9).toLongLong();
            int incoming_amount = query.value(10).toInt();
            int sold_amount = query.value(11).toInt();
            int storage_amount = query.value(12).toInt();
            int purchase = query.value(13).toInt();
            int price = query.value(14).toInt();

            InventoryItem item(
                    inventory, code.toStdString(), manufacture, shelf_life, storage_time,
                    incoming_amount, sold_amount, storage_amount, purchase, price
            );

            item.setId(id);
            item.setVersion(version);
            item.setCreateTime(createTime);
            item.setUpdateTime(updateTime);
            item.setDelete(delFlag);

            result.append(item); // 将查询到的 InventoryItem 对象添加到结果列表中
        }
    }

    return result;  // 返回包含多个 InventoryItem 对象的列表
}

QList<InventoryItem> InventoryItemMapper::getByCode(const QString &pBarcode, int maxSize) {
    QList<InventoryItem> result = {}; // 定义一个 InventoryItemList，用于存储多个 InventoryItem 对象

    QSqlQuery query;
    query.prepare(
            QString::fromStdString("SELECT * FROM ")
            + this->getTableName()
            + " WHERE code LIKE :code ORDER BY `id` DESC LIMIT :maxSize"
    );
    query.bindValue(":code", "%" + pBarcode + "%");  // 使用 % 符号来实现模糊查询
    query.bindValue(":maxSize", maxSize);
    if (query.exec()) {
        while (query.next() && result.size() < maxSize) {
            long long id = query.value(0).toLongLong();
            int version = query.value(1).toInt();
            long long createTime = query.value(2).toLongLong();
            long long updateTime = query.value(3).toLongLong();
            bool delFlag = query.value(4).toBool();
            long long inventory = query.value(5).toLongLong();
            QString code = query.value(6).toString();
            long long manufacture = query.value(7).toLongLong();
            long long shelf_life = query.value(8).toLongLong();
            long long storage_time = query.value(9).toLongLong();
            int incoming_amount = query.value(10).toInt();
            int sold_amount = query.value(11).toInt();
            int storage_amount = query.value(12).toInt();
            int purchase = query.value(13).toInt();
            int price = query.value(14).toInt();

            InventoryItem item(
                    inventory, code.toStdString(), manufacture, shelf_life, storage_time,
                    incoming_amount, sold_amount, storage_amount, purchase, price
            );

            item.setId(id);
            item.setVersion(version);
            item.setCreateTime(createTime);
            item.setUpdateTime(updateTime);
            item.setDelete(delFlag);

            result.append(item); // 将查询到的 InventoryItem 对象添加到结果列表中
        }
    }

    return result;  // 返回包含多个 InventoryItem 对象的列表
}

InventoryItem InventoryItemMapper::getById(long long pId) {
    std::stringstream sb;
    sb << "SELECT * FROM `" << this->getTableName() << "` WHERE `id`=" << pId << " AND `del_flag`=0;";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        long long inventory = query.value(5).toLongLong();
        QString code = query.value(6).toString();
        long long manufacture = query.value(7).toLongLong();
        long long shelfLife = query.value(8).toLongLong();
        long long storageTime = query.value(9).toLongLong();
        int incomingAmount = query.value(10).toInt();
        int soldAmount = query.value(11).toInt();
        int storageAmount = query.value(12).toInt();
        int purchase = query.value(13).toInt();
        int price = query.value(14).toInt();
        std::unique_ptr<InventoryItem> goods = std::make_unique<InventoryItem>(
                inventory, code.toStdString(), manufacture, shelfLife, storageTime, incomingAmount, soldAmount,
                storageAmount, purchase, price
        );
        goods->setId(id);
        goods->setVersion(version);
        goods->setCreateTime(createTime);
        goods->setUpdateTime(updateTime);
        goods->setDelete(delFlag);
        return *goods;
    }
    return InventoryItemEmpty;
}

InventoryItemMapper &InventoryItemMapper::save(InventoryItem &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("INSERT INTO `") + this->getTableName() +
            "`(`id`, `version`, `create_time`, `update_time`, `del_flag`, `inventory`, `code`, `manufacture`, `shelf_life`, `storage_time`, `incoming_amount`, `sold_amount`, `storage_amount`, `purchase`, `price`) VALUES (:id, :version, :create_time, :update_time, :del_flag, :inventory, :code, :manufacture, :shelf_life, :storage_time, :incoming_amount, :sold_amount, :storage_amount, :purchase, :price);"
    );

    query.bindValue(":id", data.getId());
    query.bindValue(":version", data.getVersion());
    query.bindValue(":create_time", Time::current());
    query.bindValue(":update_time", Time::current());
    query.bindValue(":del_flag", data.isDelete());
    query.bindValue(":inventory", data.getInventory());
    query.bindValue(":code", QString::fromStdString(data.getCode()));
    query.bindValue(":manufacture", data.getManufacture());
    query.bindValue(":shelf_life", data.getShelfLife());
    query.bindValue(":storage_time", data.getStorageTime());
    query.bindValue(":incoming_amount", data.getIncomingAmount());
    query.bindValue(":sold_amount", data.getSoldAmount());
    query.bindValue(":storage_amount", data.getStorageAmount());
    query.bindValue(":purchase", data.getPurchase());
    query.bindValue(":price", data.getPrice());

    if (!query.exec()) {
        qDebug() << "Error inserting into inventory table\n";
    }

    return *this;
}

InventoryItemMapper &InventoryItemMapper::update(InventoryItem &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "` SET `version`=:new_version, `update_time`=:update_time, `inventory`=:inventory, `code`=:code, `manufacture`=:manufacture, `shelf_life`=:shelf_life, `storage_time`=:storage_time, `incoming_amount`=:incoming_amount, `sold_amount`=:sold_amount, `storage_amount`=:storage_amount, `purchase`=:purchase, `price`=:price WHERE id=:id AND `del_flag`=0 AND `version`=:version;"
            )
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        InventoryItem inventoryItem = this->getById(data.getId());
        if (inventoryItem.isEmpty()) {
            qDebug() << "Error update type table, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", inventoryItem.getId());
        query.bindValue(":version", inventoryItem.getVersion());
        query.bindValue(":new_version", inventoryItem.getVersion() + 1);
        query.bindValue(":update_time", Time::current());

        query.bindValue(":inventory", data.getInventory());
        query.bindValue(":code", QString::fromStdString(data.getCode()));
        query.bindValue(":manufacture", data.getManufacture());
        query.bindValue(":shelf_life", data.getShelfLife());
        query.bindValue(":storage_time", data.getStorageTime());
        query.bindValue(":incoming_amount", data.getIncomingAmount());
        query.bindValue(":sold_amount", data.getSoldAmount());
        query.bindValue(":storage_amount", data.getStorageAmount());
        query.bindValue(":purchase", data.getPurchase());
        query.bindValue(":price", data.getPrice());

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error update type table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

InventoryItemMapper &InventoryItemMapper::updateByInventoryId(long long IdInventory, int quantity) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "` SET `version`=:new_version, `update_time`=:update_time, `inventory`=:inventory, `code`=:code, `manufacture`=:manufacture, `shelf_life`=:shelf_life, `storage_time`=:storage_time, `incoming_amount`=:incoming_amount, `sold_amount`=:sold_amount, `storage_amount`=:storage_amount, `purchase`=:purchase, `price`=:price WHERE id=:id AND `del_flag`=0 AND `version`=:version;"
            )
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        InventoryItem inventoryItem = this->getByInventory(IdInventory);
        if (inventoryItem.isEmpty()) {
            qDebug() << "Error update type table, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", inventoryItem.getId());
        query.bindValue(":version", inventoryItem.getVersion());
        query.bindValue(":new_version", inventoryItem.getVersion() + 1);
        query.bindValue(":update_time", Time::current());

        query.bindValue(":inventory", IdInventory);
        query.bindValue(":code", QString::fromStdString(inventoryItem.getCode()));
        query.bindValue(":manufacture", inventoryItem.getManufacture());
        query.bindValue(":shelf_life", inventoryItem.getShelfLife());
        query.bindValue(":storage_time", inventoryItem.getStorageTime());
        query.bindValue(":incoming_amount", inventoryItem.getIncomingAmount());
        query.bindValue(":sold_amount", inventoryItem.getSoldAmount() + quantity);
        query.bindValue(":storage_amount", inventoryItem.getStorageAmount() - quantity);
        query.bindValue(":purchase", inventoryItem.getPurchase());
        query.bindValue(":price", inventoryItem.getPrice());

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error update type table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

InventoryItemMapper &InventoryItemMapper::del(InventoryItem &data) {
    if (!Database::isInited())return *this;

    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + this->getTableName()
            +
            "` SET `version`=:new_version, `update_time`=:update_time, `del_flag`=:del_flag WHERE `id`=:id AND `del_flag`=0 AND `version`=:version;"
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        InventoryItem item = this->getById(data.getId());

        if (item.isEmpty()) {
            qDebug() << "Error delete item table tuple, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", item.getId());
        query.bindValue(":version", item.getVersion());
        query.bindValue(":new_version", item.getVersion() + 1);
        query.bindValue(":update_time", Time::current());
        query.bindValue(":del_flag", true);

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error delete item table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

QList<InventoryItem> InventoryItemMapper::search(const QString &keyWord, int maxSize) {
    QList<InventoryItem> result = {};
    if (!Database::isInited())return result;
    QSqlQuery query;
    if (keyWord.isEmpty()) {
        query = QSqlQuery();
        query.prepare(
                QString::fromStdString("SELECT * FROM `")
                + this->getTableName()
                + QString::fromStdString("` WHERE `del_flag`=0 ORDER BY `id` DESC LIMIT :maxSize;")
        );
        query.bindValue(":maxSize", maxSize);
    } else {
        query = QSqlQuery();
        query.prepare(
                QString::fromStdString("SELECT * FROM `")
                + this->getTableName()
                + QString::fromStdString(
                        "` WHERE `del_flag`=0 AND `code` LIKE :keyWord ORDER BY `id` DESC LIMIT :maxSize;")
        );
        query.bindValue(":keyWord", "%" + keyWord + "%");
        query.bindValue(":maxSize", maxSize);
    }
    if (!query.exec()) {
        QSqlError error = query.lastError();
        qDebug() << "Error search item table tuple:\n" << error;
        return result;
    }
    while (query.next()) {
        InventoryItem item(
                query.value("inventory").toLongLong(),
                query.value("code").toString().toStdString(),
                query.value("manufacture").toLongLong(),
                query.value("shelf_life").toLongLong(),
                query.value("storage_time").toLongLong(),
                query.value("incoming_amount").toInt(),
                query.value("sold_amount").toInt(),
                query.value("storage_amount").toInt(),
                query.value("purchase").toInt(),
                query.value("price").toInt()
        );
        item.setId(query.value("id").toLongLong());
        item.setVersion(query.value("version").toInt());
        item.setCreateTime(query.value("create_time").toLongLong());
        item.setUpdateTime(query.value("update_time").toLongLong());
        item.setDelete(query.value("del_flag").toBool());
        item.setCode(query.value("code").toString().toStdString());
        result.append(item);
    }
    return result;
}

QList<InventoryItem> InventoryItemMapper::getItemsByInventory(long long int pInventory, int maxSize) {
    QList<InventoryItem> result = {};
    if (!Database::isInited())return result;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("SELECT * FROM `")
            + this->getTableName()
            + QString::fromStdString(
                    "` WHERE `del_flag`=0 AND `inventory`=:inventory ORDER BY `id` DESC LIMIT :maxSize;")
    );
    query.bindValue(":inventory", pInventory);
    query.bindValue(":maxSize", maxSize);
    if (!query.exec()) {
        QSqlError error = query.lastError();
        qDebug() << "Error search item table tuple:\n" << error;
        return result;
    }
    while (query.next()) {
        InventoryItem item(
                query.value("inventory").toLongLong(),
                query.value("code").toString().toStdString(),
                query.value("manufacture").toLongLong(),
                query.value("shelf_life").toLongLong(),
                query.value("storage_time").toLongLong(),
                query.value("incoming_amount").toInt(),
                query.value("sold_amount").toInt(),
                query.value("storage_amount").toInt(),
                query.value("purchase").toInt(),
                query.value("price").toInt()
        );
        item.setId(query.value("id").toLongLong());
        item.setVersion(query.value("version").toInt());
        item.setCreateTime(query.value("create_time").toLongLong());
        item.setUpdateTime(query.value("update_time").toLongLong());
        item.setDelete(query.value("del_flag").toBool());
        item.setCode(query.value("code").toString().toStdString());
        result.append(item);
    }
    return result;
}

ShoppingList::ShoppingList(std::string pCode, long long int pConfirmTime, int pPrice, int pPayment, int pChange) :
        DataEntity(), code(std::move(pCode)), confirmTime(pConfirmTime), price(pPrice), payment(pPayment),
        change(pChange) {}

std::string ShoppingList::getCode() const {
    return this->code;
}

ShoppingList &ShoppingList::setCode(std::string pCode) {
    this->code = std::move(pCode);
    return *this;
}

long long ShoppingList::getConfirmTime() const {
    return this->confirmTime;
}

ShoppingList &ShoppingList::setConfirmTime(long long int pConfirmTime) {
    this->confirmTime = pConfirmTime;
    return *this;
}

int ShoppingList::getPrice() const {
    return this->price;
}

ShoppingList &ShoppingList::setPrice(int pPrice) {
    this->price = pPrice;
    return *this;
}

int ShoppingList::getPayment() const {
    return this->payment;
}

ShoppingList &ShoppingList::setPayment(int pPayment) {
    this->payment = pPayment;
    return *this;
}

int ShoppingList::getChange() const {
    return this->change;
}

ShoppingList &ShoppingList::setChange(int pChange) {
    this->change = pChange;
    return *this;
}

bool ShoppingList::isEmpty() {
    return this == &ShoppingListEmpty;
}

const char *ShoppingListMapper::getTableName() {
    return "shopping_list";
}

bool ShoppingListMapper::createTable() {
    QString qString = "create table `shopping_list`\n"
                      "(\n"
                      "    `id`           INTEGER           not null\n"   // 标识符
                      "        constraint shopping_list_pk primary key,\n"
                      "    `version`      INTEGER default 0 not null,\n"  // 乐观锁
                      "    `create_time`  INTEGER           not null,\n"  // 创建时间
                      "    `update_time`  INTEGER           not null,\n"  // 更新时间
                      "    `del_flag`     INTEGER default 0 not null,\n"  // 逻辑删除
                      "    `code`         TEXT              not null,\n"  // 小票号
                      "    `confirm_time` INTEGER           not null,\n"  // 确认时间
                      "    `price`        INTEGER           not null,\n"  // 应付
                      "    `payment`      INTEGER           not null,\n"  // 实付
                      "    `change`       INTEGER           not null\n"   // 找零
                      ");";
    QSqlQuery query;
    query.prepare(qString);
    if (!query.exec()) {
        qDebug() << "Error create shopping_list table\n";
        return false;
    }
    return true;
}

ShoppingList ShoppingListMapper::getById(long long int pId) {
    std::stringstream sb;
    sb << "SELECT * FROM `" << this->getTableName() << "` WHERE `id`=" << pId << " AND `del_flag`=0;";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        QString code = query.value(5).toString();
        long long confirmTime = query.value(6).toLongLong();
        int price = query.value(7).toInt();
        int payment = query.value(8).toInt();
        int change = query.value(9).toInt();
        std::unique_ptr<ShoppingList> shoppingList = std::make_unique<ShoppingList>(code.toStdString(), confirmTime,
                                                                                    price, payment, change);
        shoppingList->setId(id);
        shoppingList->setVersion(version);
        shoppingList->setCreateTime(createTime);
        shoppingList->setUpdateTime(updateTime);
        shoppingList->setDelete(delFlag);
        return *shoppingList;
    }
    return ShoppingListEmpty;
}

ShoppingListMapper &ShoppingListMapper::save(ShoppingList &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("INSERT INTO `") + this->getTableName() +
            "`(id, version, create_time, update_time, del_flag, code, confirm_time, price, payment, change) VALUES (:id, :version, :create_time, :update_time, :del_flag, :code, :confirm_time, :price, :payment, :change);"
    );

    query.bindValue(":id", data.getId());
    query.bindValue(":version", data.getVersion());
    query.bindValue(":create_time", Time::current());
    query.bindValue(":update_time", Time::current());
    query.bindValue(":del_flag", data.isDelete());
    query.bindValue(":code", QString::fromStdString(data.getCode()));
    query.bindValue(":confirm_time", data.getConfirmTime());
    query.bindValue(":price", data.getPrice());
    query.bindValue(":change", data.getChange());

    if (!query.exec()) {
        qDebug() << "Error inserting into inventory table\n";
    }

    return *this;
}

ShoppingListMapper &ShoppingListMapper::update(ShoppingList &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "` SET `version`=:new_version, `update_time`=:update_time, `code`=:code, `confirm_time`=:confirm_time, `price`=:price, `change`=:change WHERE id=:id AND `del_flag`=0 AND `version`=:version;"
            )
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        ShoppingList shoppingList = this->getById(data.getId());
        if (shoppingList.isEmpty()) {
            qDebug() << "Error update type table, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", shoppingList.getId());
        query.bindValue(":version", shoppingList.getVersion());
        query.bindValue(":new_version", shoppingList.getVersion() + 1);
        query.bindValue(":update_time", Time::current());

        query.bindValue(":code", QString::fromStdString(data.getCode()));
        query.bindValue(":confirm_time", data.getConfirmTime());
        query.bindValue(":price", data.getPrice());
        query.bindValue(":change", data.getChange());

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error update type table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

ShoppingListMapper &ShoppingListMapper::del(ShoppingList &data) {
    if (!Database::isInited())return *this;

    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + this->getTableName()
            +
            "` SET `version`=:new_version, `update_time`=:update_time, `del_flag`=:del_flag WHERE `id`=:id AND `del_flag`=0 AND `version`=:version;"
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        ShoppingList list = this->getById(data.getId());

        if (list.isEmpty()) {
            qDebug() << "Error delete list table tuple, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", list.getId());
        query.bindValue(":version", list.getVersion());
        query.bindValue(":new_version", list.getVersion() + 1);
        query.bindValue(":update_time", Time::current());
        query.bindValue(":del_flag", true);

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error delete list table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

ShoppingListItem::ShoppingListItem(long long int pShoppingList, long long int pInventoryItem, int pPrice, int pAmount)
        : DataEntity(), shoppingList(pShoppingList), inventoryItem(pInventoryItem), price(pPrice), amount(pAmount) {}

long long ShoppingListItem::getShoppingList() const {
    return this->shoppingList;
}

ShoppingListItem &ShoppingListItem::setShoppingList(long long int pShoppingList) {
    this->shoppingList = pShoppingList;
    return *this;
}

long long ShoppingListItem::getInventoryItem() const {
    return this->inventoryItem;
}

ShoppingListItem &ShoppingListItem::setInventoryItem(long long int pInventoryItem) {
    this->inventoryItem = pInventoryItem;
    return *this;
}

int ShoppingListItem::getPrice() const {
    return this->price;
}

ShoppingListItem &ShoppingListItem::setPrice(int pPrice) {
    this->price = pPrice;
    return *this;
}

int ShoppingListItem::getAmount() const {
    return this->amount;
}

ShoppingListItem &ShoppingListItem::setAmount(int pAmount) {
    this->amount = pAmount;
    return *this;
}

bool ShoppingListItem::isEmpty() {
    return this == &ShoppingListItemEmpty;
}

const char *ShoppingListItemMapper::getTableName() {
    return "shopping_list_item";
}

bool ShoppingListItemMapper::createTable() {
    QString qString = "create table `shopping_list_item`\n"
                      "(\n"
                      "    `id`             INTEGER           not null\n"   // 标识符
                      "        constraint shopping_list_item_pk primary key,\n"
                      "    `version`        INTEGER default 0 not null,\n"  // 乐观锁
                      "    `create_time`    INTEGER           not null,\n"  // 创建时间
                      "    `update_time`    INTEGER           not null,\n"  // 更新时间
                      "    `del_flag`       INTEGER default 0 not null,\n"  // 逻辑删除
                      "    `shopping_list`  INTEGER           not null,\n"  // 购物单标识符
                      "    `inventory_item` INTEGER           not null,\n"  // 库存项标识符
                      "    `price`          INTEGER           not null,\n"  // 商品价格
                      "    `amount`         INTEGER           not null\n"   // 商品数量
                      ");";
    QSqlQuery query;
    query.prepare(qString);
    if (!query.exec()) {
        qDebug() << "Error create  table\n";
        return false;
    }
    return true;
}

ShoppingListItem ShoppingListItemMapper::getById(long long int pId) {
    std::stringstream sb;
    sb << "SELECT * FROM `" << this->getTableName() << "` WHERE `id`=" << pId << " AND `del_flag`=0;";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        long long shoppingList = query.value(5).toLongLong();
        long long inventoryItem = query.value(6).toLongLong();
        int price = query.value(7).toInt();
        int amount = query.value(8).toInt();
        std::unique_ptr<ShoppingListItem> listItem = std::make_unique<ShoppingListItem>(shoppingList, inventoryItem,
                                                                                        price, amount);
        listItem->setId(id);
        listItem->setVersion(version);
        listItem->setCreateTime(createTime);
        listItem->setUpdateTime(updateTime);
        listItem->setDelete(delFlag);
        return *listItem;
    }
    return ShoppingListItemEmpty;
}

ShoppingListItemMapper &ShoppingListItemMapper::save(ShoppingListItem &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("INSERT INTO `") + this->getTableName() +
            "`(id, version, create_time, update_time, del_flag, shopping_list, inventory_item, price, amount) VALUES (:id, :version, :create_time, :update_time, :del_flag, :shopping_list, :inventory_item, :price, :amount);"
    );

    query.bindValue(":id", data.getId());
    query.bindValue(":version", data.getVersion());
    query.bindValue(":create_time", Time::current());
    query.bindValue(":update_time", Time::current());
    query.bindValue(":del_flag", data.isDelete());

    query.bindValue(":shopping_list", data.getShoppingList());
    query.bindValue(":inventory_item", data.getInventoryItem());
    query.bindValue(":price", data.getPrice());
    query.bindValue(":amount", data.getAmount());

    if (!query.exec()) {
        qDebug() << "Error inserting into inventory table\n";
    }

    return *this;
}

ShoppingListItemMapper &ShoppingListItemMapper::update(ShoppingListItem &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "` SET `version`=:new_version, `update_time`=:update_time, `shopping_list`=:shopping_list, `inventory_item`=:inventory_item, `price`=:price, `amount`=:amount WHERE id=:id AND `del_flag`=0 AND `version`=:version;"
            )
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        ShoppingListItem listItem = this->getById(data.getId());
        if (listItem.isEmpty()) {
            qDebug() << "Error update type table, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", listItem.getId());
        query.bindValue(":version", listItem.getVersion());
        query.bindValue(":new_version", listItem.getVersion() + 1);
        query.bindValue(":update_time", Time::current());

        query.bindValue(":shopping_list", data.getShoppingList());
        query.bindValue(":inventory_item", data.getInventoryItem());
        query.bindValue(":price", data.getPrice());
        query.bindValue(":amount", data.getAmount());

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error update type table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

ShoppingListItemMapper &ShoppingListItemMapper::del(ShoppingListItem &data) {
    if (!Database::isInited())return *this;

    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + this->getTableName()
            +
            "` SET `version`=:new_version, `update_time`=:update_time, `del_flag`=:del_flag WHERE `id`=:id AND `del_flag`=0 AND `version`=:version;"
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        ShoppingListItem listItem = this->getById(data.getId());

        if (listItem.isEmpty()) {
            qDebug() << "Error delete listItem table tuple, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", listItem.getId());
        query.bindValue(":version", listItem.getVersion());
        query.bindValue(":new_version", listItem.getVersion() + 1);
        query.bindValue(":update_time", Time::current());
        query.bindValue(":del_flag", true);

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error delete listItem table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

ReturnOfGoods::ReturnOfGoods(long long int pShoppingList, long long int pInventoryItem, int pPrice, int pAmount)
        : DataEntity(), shoppingList(pShoppingList), inventoryItem(pInventoryItem), price(pPrice), amount(pAmount) {}

long long ReturnOfGoods::getShoppingList() const {
    return this->shoppingList;
}

ReturnOfGoods &ReturnOfGoods::setShoppingList(long long int pShoppingList) {
    this->shoppingList = pShoppingList;
    return *this;
}

long long ReturnOfGoods::getInventoryItem() const {
    return this->inventoryItem;
}

ReturnOfGoods &ReturnOfGoods::setInventoryItem(long long int pInventoryItem) {
    this->inventoryItem = pInventoryItem;
    return *this;
}

int ReturnOfGoods::getPrice() const {
    return this->price;
}

ReturnOfGoods &ReturnOfGoods::setPrice(int pPrice) {
    this->price = pPrice;
    return *this;
}

int ReturnOfGoods::getAmount() const {
    return this->amount;
}

ReturnOfGoods &ReturnOfGoods::setAmount(int pAmount) {
    this->amount = pAmount;
    return *this;
}

bool ReturnOfGoods::isEmpty() {
    return this == &ReturnOfGoodsEmpty;
}

const char *ReturnOfGoodsItemMapper::getTableName() {
    return "return_of_goods";
}

bool ReturnOfGoodsItemMapper::createTable() {
    QString qString = "create table `return_of_goods`\n"
                      "(\n"
                      "    `id`             INTEGER           not null\n"   // 标识符
                      "        constraint return_of_goods_pk primary key,\n"
                      "    `version`        INTEGER default 0 not null,\n"  // 乐观锁
                      "    `create_time`    INTEGER           not null,\n"  // 创建时间
                      "    `update_time`    INTEGER           not null,\n"  // 更新时间
                      "    `del_flag`       INTEGER default 0 not null,\n"  // 逻辑删除
                      "    `shopping_list`  INTEGER           not null,\n"  // 购物单标识符
                      "    `inventory_item` INTEGER           not null,\n"  // 库存项标识符
                      "    `price`          INTEGER           not null,\n"  // 商品价格
                      "    `amount`         INTEGER           not null\n"   // 退货数量
                      ");";
    QSqlQuery query;
    query.prepare(qString);
    if (!query.exec()) {
        qDebug() << "Error create  table\n";
        return false;
    }
    return true;
}

ReturnOfGoods ReturnOfGoodsItemMapper::getById(long long int pId) {
    std::stringstream sb;
    sb << "SELECT * FROM `" << this->getTableName() << "` WHERE `id`=" << pId << " AND `del_flag`=0;";
    QSqlQuery query(QString::fromStdString(sb.str()));
    while (query.next()) {
        long long id = query.value(0).toLongLong();
        int version = query.value(1).toInt();
        long long createTime = query.value(2).toLongLong();
        long long updateTime = query.value(3).toLongLong();
        bool delFlag = query.value(4).toBool();
        long long shoppingList = query.value(5).toLongLong();
        long long inventoryItem = query.value(6).toLongLong();
        int price = query.value(7).toInt();
        int amount = query.value(8).toInt();
        std::unique_ptr<ReturnOfGoods> goods = std::make_unique<ReturnOfGoods>(shoppingList, inventoryItem,
                                                                               price, amount);
        goods->setId(id);
        goods->setVersion(version);
        goods->setCreateTime(createTime);
        goods->setUpdateTime(updateTime);
        goods->setDelete(delFlag);
        return *goods;
    }
    return ReturnOfGoodsEmpty;
}

ReturnOfGoodsItemMapper &ReturnOfGoodsItemMapper::save(ReturnOfGoods &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("INSERT INTO `") + this->getTableName() +
            "`(id, version, create_time, update_time, del_flag, shopping_list, inventory_item, price, amount) VALUES (:id, :version, :create_time, :update_time, :del_flag, :shopping_list, :inventory_item, :price, :amount);"
    );

    query.bindValue(":id", data.getId());
    query.bindValue(":version", data.getVersion());
    query.bindValue(":create_time", Time::current());
    query.bindValue(":update_time", Time::current());
    query.bindValue(":del_flag", data.isDelete());
    query.bindValue(":shopping_list", data.getShoppingList());
    query.bindValue(":inventory_item", data.getInventoryItem());
    query.bindValue(":price", data.getPrice());
    query.bindValue(":amount", data.getAmount());

    if (!query.exec()) {
        qDebug() << "Error inserting into inventory table\n";
    }

    return *this;
}

ReturnOfGoodsItemMapper &ReturnOfGoodsItemMapper::update(ReturnOfGoods &data) {
    if (!Database::isInited())return *this;
    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + QString::fromStdString(this->getTableName())
            + QString::fromStdString(
                    "` SET `version`=:new_version, `update_time`=:update_time, `shopping_list`=:shopping_list, `inventory_item`=:inventory_item, `price`=:price, `amount`=:amount WHERE id=:id AND `del_flag`=0 AND `version`=:version;"
            )
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        ReturnOfGoods goods = this->getById(data.getId());
        if (goods.isEmpty()) {
            qDebug() << "Error update type table, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", goods.getId());
        query.bindValue(":version", goods.getVersion());
        query.bindValue(":new_version", goods.getVersion() + 1);
        query.bindValue(":update_time", Time::current());

        query.bindValue(":shopping_list", data.getShoppingList());
        query.bindValue(":inventory_item", data.getInventoryItem());
        query.bindValue(":price", data.getPrice());
        query.bindValue(":amount", data.getAmount());

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error update type table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}

ReturnOfGoodsItemMapper &ReturnOfGoodsItemMapper::del(ReturnOfGoods &data) {
    if (!Database::isInited())return *this;

    QSqlQuery query = QSqlQuery();
    query.prepare(
            QString::fromStdString("UPDATE `")
            + this->getTableName()
            +
            "` SET `version`=:new_version, `update_time`=:update_time, `del_flag`=:del_flag WHERE `id`=:id AND `del_flag`=0 AND `version`=:version;"
    );

    QSqlError::ErrorType errorType = QSqlError::ErrorType::NoError;

    do {
        ReturnOfGoods goods = this->getById(data.getId());

        if (goods.isEmpty()) {
            qDebug() << "Error delete goods table tuple, the specified data does not exist\n";
            return *this;
        }

        query.bindValue(":id", goods.getId());
        query.bindValue(":version", goods.getVersion());
        query.bindValue(":new_version", goods.getVersion() + 1);
        query.bindValue(":update_time", Time::current());
        query.bindValue(":del_flag", true);

        if (!query.exec()) {
            QSqlError error = query.lastError();
            errorType = error.type();
            qDebug() << "Error delete goods table tuple:\n" << error;
        }
    } while (errorType == QSqlError::ErrorType::TransactionError);

    return *this;
}
