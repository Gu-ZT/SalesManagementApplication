#include "../ui_InventoryInterface.h"
#include "InventoryInterface.h"

InventoryInterface::InventoryInterface(QWidget *parent)
        : QWidget(parent), AbstractInterface(), ui(new Ui::InventoryInterface) {
    ui->setupUi(this);
    connect(ui->pbtnSortSelect, &QPushButton::clicked, this, &InventoryInterface::quantityProcess);
    connect(ui->backBtn, &QPushButton::clicked, this, [this] {
        this->getMainWindow()->showMaximized();
        this->close();
    });
}

InventoryInterface::~InventoryInterface() {
    delete ui;
}

//设置下拉框选项
void InventoryInterface::addComboBoxItems() {

    InventoryTypeMapper mapperType;

    QStringList items = mapperType.getType();
    ui->cbsortSelect->addItems(items);
    ui->cbsortSelect->setCurrentIndex(0);
}

void InventoryInterface::quantityProcess() {
    // 清除所有行和列
    ui->twInventory->clearContents(); // 清除所有单元格的内容
    ui->twInventory->setRowCount(0);  // 设置行数为0，清空所有行
    ui->twInventory->setColumnCount(0); // 设置列数为0，清空所有列
    QString currentText = ui->cbsortSelect->currentText();//获取当前pbtnSortSelect的内容

    InventoryTypeMapper mapperType;
    InventoryItemMapper mapperInventoryItem;
    InventoryMapper mapperInventory;

    InventoryType InventoryTypeObject = mapperType.getByType(currentText.toStdString());

    long long id = InventoryTypeObject.getId();//在type表中获得该类型的标识符

    QList<Inventory> InventoryObiect = mapperInventory.getByType(id);
    QList<Inventory>::iterator it = InventoryObiect.begin();
    for (; it != InventoryObiect.end(); ++it) {
        long long id = it->getId();
        std::string productName = it->getName();
        InventoryItem InventoryItemObject = mapperInventoryItem.getByInventory(id);
        long long manufacture = InventoryItemObject.getManufacture();
        long long shelfLife = InventoryItemObject.getShelfLife();
        long long storageTime = InventoryItemObject.getStorageTime();
        long long incomingAmount = InventoryItemObject.getIncomingAmount();
        long long soldAmount = InventoryItemObject.getSoldAmount();
        long long storageAmount = InventoryItemObject.getStorageAmount();
        QString barcode = QString::fromStdString(InventoryItemObject.getCode());
        tableWidgetDisplay(QString::fromStdString(productName), barcode, currentText, storageTime, manufacture,
                           shelfLife, incomingAmount, soldAmount, storageAmount);
    }


}

void InventoryInterface::tableWidgetDisplay(const QString &name, const QString &code, const QString &type,
                                            long long storageTime, long long manufacture, long long shelfLife,
                                            long long incomingAmount, long long soldAmount, long long storageAmount) {
    // 获取当前表格的行数
    int row = ui->twInventory->rowCount();

    // 设置行号（序号）列
    QTableWidgetItem *serialItem = new QTableWidgetItem(QString::number(row + 1));
    ui->twInventory->setItem(row, 0, serialItem);

    // 设置入库时间列
    QTableWidgetItem *storageTimeItem = new QTableWidgetItem(QString::number(storageTime));
    ui->twInventory->setItem(row, 1, storageTimeItem);

    // 设置类别列
    QTableWidgetItem *typeTimeItem = new QTableWidgetItem(type);
    ui->twInventory->setItem(row, 2, typeTimeItem);


    // 设置商品名字列
    QTableWidgetItem *nameItem = new QTableWidgetItem(name);
    ui->twInventory->setItem(row, 3, nameItem);

    //设置商品条码列
    QTableWidgetItem *codeItem = new QTableWidgetItem(code);
    ui->twInventory->setItem(row, 4, codeItem);

    // 设置入库数量列
    QTableWidgetItem *incomingAmountItem = new QTableWidgetItem(QString::number(incomingAmount));
    ui->twInventory->setItem(row, 5, incomingAmountItem);

    // 设置销售数量列
    QTableWidgetItem *soldAmountItem = new QTableWidgetItem(QString::number(soldAmount));
    ui->twInventory->setItem(row, 6, soldAmountItem);

    // 设置库存数量列
    QTableWidgetItem *storageAmountItem = new QTableWidgetItem(QString::number(storageAmount));
    ui->twInventory->setItem(row, 7, storageAmountItem);

    // 设置生产日期列
    QTableWidgetItem *manufactureItem = new QTableWidgetItem(QString::number(manufacture));
    ui->twInventory->setItem(row, 8, manufactureItem);

    // 设置保质期列
    QTableWidgetItem *shelfLifeItem = new QTableWidgetItem(QString::number(shelfLife));
    ui->twInventory->setItem(row, 9, shelfLifeItem);
}

void InventoryInterface::quantitydateProcess() {
    QDateTime dateTimestart = ui->dteTimeStart->dateTime();

    // 将日期时间转换为 QString 类型
    QString dateTimeStr = dateTimestart.toString("yyyy-MM-dd hh:mm:ss");

    QDateTime dateTimeend = ui->dteTimeEnd->dateTime();

    // 将日期时间转换为 QString 类型
    QString dateTimeen = dateTimeend.toString("yyyy-MM-dd hh:mm:ss");
    InventoryItemMapper mapperInventoryItem;
    InventoryMapper mapperInventory;
    InventoryTypeMapper mapperType;

    QList<InventoryItem> list = mapperInventoryItem.getByDate(dateTimeStr, dateTimeen);

    QList<InventoryItem>::iterator it = list.begin();
    for (; it != list.end(); ++it) {
        long long inventoryId = it->getInventory();
        long long manufacture = it->getManufacture();
        long long shelfLife = it->getShelfLife();
        long long storageTime = it->getStorageTime();
        long long incomingAmount = it->getIncomingAmount();
        long long soldAmount = it->getSoldAmount();
        long long storageAmount = it->getStorageAmount();
        QString barcode = QString::fromStdString(it->getCode());
        Inventory inventoryObject = mapperInventory.getById(inventoryId);
        std::string productName = inventoryObject.getName();
        long long typePid = inventoryObject.getType();
        InventoryType typeObject = mapperType.getById(typePid);
        std::string type = typeObject.getName();
        tableWidgetDisplay(QString::fromStdString(productName), barcode, QString::fromStdString(type), storageTime,
                           manufacture, shelfLife, incomingAmount, soldAmount, storageAmount);
    }

}


