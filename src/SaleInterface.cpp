#include <QStringListModel>
#include <utility>
#include <QSpinBox>
#include <QUuid>

#include "cashpaymentdialog.h"
#include "SaleInterface.h"
#include "../ui_SaleInterface.h"
#include "EnterInterface.h"
#include "sacr.h"
#include <QMessageBox>

//用于显示例如¥3/个的格式
class PriceUint {
private:
    double value;//商品价格
    QString unit;//产品单位
public:
    // 构造函数
    PriceUint(double priceValue, QString priceUnit)
            : value(priceValue), unit(std::move(priceUnit)) {}

    // 获取价格的字符串表示，例如¥3/个
    [[nodiscard]] QString toString() const {
        return QString("¥%1/%2").arg(value).arg(unit);
    }
};

// 定义一个结构体来存储每个SpinBox的信息
class SpinBoxInfo {
private:
    QSpinBox *spinBox;  // SpinBox 对象指针
    int row;            // 表格中的行号
    double price;       // 商品价格

public:
    // 构造函数，方便初始化 SpinBoxInfo 对象
    SpinBoxInfo(QSpinBox *sb, int pRow, double pPrice) : spinBox(sb), row(pRow), price(pPrice) {}

    //用来处理对spinbox值变化的处理
    static void updateTotalPrice(int pRow, double pPrice, QSpinBox *quantitySpinBox, QTableWidget *tableWidget);

    void setupSpinBoxesAndConnections(int pRow, QList<SpinBoxInfo> &pSpinBoxes, QTableWidget *tableWidget);
};

//商品信息类
class ProductInformation : public PriceUint {
private:
    QString productName;
public:
    ProductInformation(double pPriceValue, QString pPriceUnit, QString pProductName) :
            PriceUint(pPriceValue, std::move(pPriceUnit)) {
        this->productName = std::move(pProductName);
    }
};

QList<SpinBoxInfo> spinBoxes = {};  // 定义一个 QList 来存储 SpinBoxInfo
QList<ProductInformation> ProductInformation;//存储返回的商品信息

SaleInterface::SaleInterface(QMainWindow *parent) : QMainWindow(parent), ui(new Ui::SaleInterface) {
    ui->setupUi(this);

    //设置QLineEdit文本显示格式为正常显示
    ui->search->setEchoMode(QLineEdit::Normal);
    ui->search->selectAll();
    // 初始化页面
    this->enterInterface = new EnterInterface();
    this->enterInterface->setMainWindow(this);
    this->inventoryInterface = new InventoryInterface();
    this->inventoryInterface->setMainWindow(this);
    this->statementInterface = new StatementInterface();
    this->statementInterface->setMainWindow(this);
    this->returnInterface = new ReturnInterface();
    this->returnInterface->setMainWindow(this);
    //connect函数将ui-search的returnpressed信号连接到on_barcodeScanned函数
    connect(ui->search, &QLineEdit::returnPressed, this, &SaleInterface::onBarcodeScanned);

    connect(ui->enter, &QPushButton::clicked, [this]() {
        this->enterInterface->initInterface();
        this->enterInterface->showMaximized();
        this->close();
    });

    connect(ui->check, &QPushButton::clicked, [this]() {
        this->inventoryInterface->initInterface();
        this->inventoryInterface->addComboBoxItems();
        this->inventoryInterface->showMaximized();
        this->close();
    });

    connect(ui->statement, &QPushButton::clicked, [this]() {
        this->statementInterface->initInterface();
        this->statementInterface->showMaximized();
        this->close();
    });

    connect(ui->salesReturn, &QPushButton::clicked, [this]() {
        this->returnInterface->initInterface();
        this->returnInterface->show();
    });

    connect(ui->scanToPay, &QPushButton::clicked, this, &SaleInterface::openSacrDialog);
    // 在设置单元格之后连接信号与槽
    QObject::connect(ui->list, &QTableWidget::cellChanged, this, &SaleInterface::updateTotalPrice);

    // 注册监听"flush"事件，当收到"flush"信号时执行flush方法
    this->listen("flush", [this] {
        this->pushQuantity();
    });
}

SaleInterface::~SaleInterface() {
    delete ui;
}

//处理输入条码
void SaleInterface::onBarcodeScanned() {
    QString barcode = ui->search->text().trimmed(); // 获取扫描到的条形码并去除两端空白

    if (!barcode.isEmpty()) {
        // 将文本框中的内容全部选中，以便下次扫码时覆盖旧条码
        ui->search->selectAll();

        // 处理条码数据
        processBarcode(barcode);
    }
}

// 处理条码数据
void SaleInterface::processBarcode(const QString &barcode) {
    InventoryItemMapper mapper;
    InventoryMapper mapper1;

    inventoryItems = mapper.getByCode(barcode);
    QList<InventoryItem>::iterator it = inventoryItems.begin();


    ui->selectList->clearSelection();
    auto *model = new QStringListModel();
    QStringList dataList;

    // 如果 inventoryItems 中只有一个商品，直接显示在 TableWidget 上
    if (inventoryItems.size() == 1) {
        const InventoryItem &item = inventoryItems.at(0);
        long long itemId = item.getInventory();
        double price = static_cast<double>(item.getPrice()) / 100.00;

        Inventory inventory = mapper1.getById(itemId);
        std::string name = inventory.getName();
        std::string spec = inventory.getSpec();

        PriceUint myPrice(price, QString::fromStdString(spec));
        QString priceString = myPrice.toString();
        QString itemName = QString::fromStdString(name);

        dataList << itemName + "\t\t" + priceString;
        model->setStringList(dataList);
        ui->selectList->setModel(model);
        ui->selectList->setStyleSheet("font-size: 20px;");
        ui->selectList->show();

        TableWidget_display(price, name, spec);
    } else {
        // 如果有多个商品，显示在 ListView 上，并等待双击事件处理
        for (; it != inventoryItems.end(); ++it) {
            long long itemId = it->getInventory();
            double price = static_cast<double>(it->getPrice()) / 100.00;

            Inventory inventory = mapper1.getById(itemId);
            std::string name = inventory.getName();
            std::string spec = inventory.getSpec();

            PriceUint myPrice(price, QString::fromStdString(spec));
            QString priceString = myPrice.toString();
            QString itemName = QString::fromStdString(name);

            dataList << itemName + "\t\t" + priceString;
        }

        model->setStringList(dataList);
        ui->selectList->setModel(model);
        ui->selectList->setStyleSheet("font-size: 20px;");
        ui->selectList->show();

        // 连接双击事件，处理显示到 TableWidget 的逻辑
        connect(ui->selectList, &QListView::doubleClicked, this, [=](const QModelIndex &index){
            InventoryMapper mapper1;

            InventoryItem it = inventoryItems.at(index.row());

            long long itemId = it.getInventory();
            double price = static_cast<double>(it.getPrice()) / 100.00;

            Inventory inventory = mapper1.getById(itemId);
            std::string name = inventory.getName();
            std::string spec = inventory.getSpec();

            TableWidget_display(price,name,spec);
        });
    }
}

//将商品信息显示在清单列表上
void SaleInterface::TableWidget_display(double price, const std::string &name, const std::string &spec) {
    PriceUint myPrice(price, QString::fromStdString(spec));
    QString priceString = myPrice.toString();

    // 在清单列表中显示商品信息
    // 先获取当前行数
    int row = ui->list->rowCount();

    // 设置新的行数
    ui->list->setRowCount(row + 1);

    // 创建 QTableWidgetItem 来显示商品名称和价格
    auto *productNameItem = new QTableWidgetItem(QString::fromStdString(name));
    auto *priceItem = new QTableWidgetItem(priceString);

    productNameItem->setFlags(productNameItem->flags() & (~Qt::ItemIsEditable));
    priceItem->setFlags(priceItem->flags() & (~Qt::ItemIsEditable));

    // 将 QTableWidgetItem 放入 tableWidget 的指定行和列
    ui->list->setItem(row, 0, productNameItem); // 第一列显示商品名称
    ui->list->setItem(row, 1, priceItem);      // 第二列显示价格

    // 创建一个 QSpinBox，用于选择商品数量
    auto *quantitySpinBox = new QSpinBox();
    quantitySpinBox->setRange(1, 999); // 设置数量范围
    quantitySpinBox->setValue(1);      // 默认数量为1

    // 设置 QSpinBox 的对象名称，可以基于行号设置唯一的名称
    QString spinBoxName = QString("quantitySpinBox_%1").arg(row); // 例如：quantitySpinBox_0, quantitySpinBox_1, ...
    quantitySpinBox->setObjectName(spinBoxName);

    // 将 QSpinBox 放入 tableWidget 的指定行和列
    ui->list->setCellWidget(row, 2, quantitySpinBox); // 第三列放置选择数量的SpinBox

    //将该行的spinbox信息存储起来，便于后续对该行进行修改
    SpinBoxInfo info = SpinBoxInfo(quantitySpinBox, row, price);
    spinBoxes.append(info);

    info.setupSpinBoxesAndConnections(row, spinBoxes, ui->list);

    connect(ui->cashPay, &QPushButton::clicked, this, &SaleInterface::on_cashPay_clicked);
}

// 来处理SpinBox的更新
void SpinBoxInfo::setupSpinBoxesAndConnections(int pRow, QList<SpinBoxInfo> &pSpinBoxes, QTableWidget *tableWidget) {
    SpinBoxInfo info = pSpinBoxes[pRow];
    // 初始化每个SpinBox的总价
    updateTotalPrice(info.row, info.price, info.spinBox, tableWidget);

    // 连接SpinBox值变化的信号与槽
    QMainWindow::connect(info.spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int) {
        updateTotalPrice(info.row, info.price, info.spinBox, tableWidget);
    });
}


// 函数来更新单个商品总价
void SpinBoxInfo::updateTotalPrice(int pRow, double pPrice, QSpinBox *quantitySpinBox, QTableWidget *tableWidget) {
    // 获取数量SpinBox的当前值
    int quantity = quantitySpinBox->value();

    // 计算商品总价
    double total = quantity * pPrice;

    // 创建QTableWidgetItem
    auto *item = new QTableWidgetItem;

    // 设置数据为 double 类型
    item->setData(Qt::EditRole, total);

    // 设置单元格不可编辑
    item->setFlags(item->flags() & (~Qt::ItemIsEditable));

    // 在表格的指定行(PRow)和列(3)中设置该QTableWidgetItem
    tableWidget->setItem(pRow, 3, item);
}

void SaleInterface::updateTotalPrice() {
    totalPrice = 0.0; // 重置总价格
    for (int row = 0; row < ui->list->rowCount(); ++row) {
        totalPrice += calculateRowTotal(row); // 累加每行的总价格
    }

    ui->price->setText(QString("¥%1").arg(totalPrice));
}

void SaleInterface::openSacrDialog() {
    Sacr *sacrDialog = new Sacr(this);
    sacrDialog->exec(); // 显示模态对话框
}

double SaleInterface::calculateRowTotal(int row) {
    QTableWidgetItem* item = ui->list->item(row, 3);
    if (item) {
        return item->data(Qt::EditRole).toDouble();
    }
    return 0.0;
}
void SaleInterface::pushQuantity() {
    int rowCount = ui->list->rowCount();

    // 生成UUID
    QUuid ticketId = QUuid::createUuid();

    // 将UUID转换为字符串形式的小票号
    QString ticketNumberQt = ticketId.toString(); // 这里的类型是QString

    std::string ticketNumber = ticketNumberQt.toUtf8().toStdString();

    double totalPrice = 0.00;
    for(int row = 0;row < rowCount; ++row){
        double price = calculateRowTotal(row);
        totalPrice = totalPrice + price;
    }

    // 获取当前时间的QDateTime对象
    QDateTime currentDateTime = QDateTime::currentDateTime();

    // 将QDateTime转换为自Unix纪元以来的毫秒数
    qint64 millisecondsSinceEpoch = currentDateTime.toMSecsSinceEpoch();

    // 将qint64转换为long long int，通常qint64已经足够大，可以存储这个值
    long long int millisecondsSinceEpochLL = static_cast<long long int>(millisecondsSinceEpoch);

    int pChange = 0;

    //生成购物单表
    ShoppingList ShoppingListObject(ticketNumber,millisecondsSinceEpochLL,totalPrice,totalPrice,pChange);
    long long ShoppingListId =ShoppingListObject.getId();

    for (int row = 0; row < rowCount; ++row) {
        // 获取商品名称和价格的 QTableWidgetItem
        QTableWidgetItem *nameItem = ui->list->item(row, 0);
        QTableWidgetItem *priceItem = ui->list->item(row, 1);

        // 获取数量选择器 QSpinBox
        QSpinBox *quantitySpinBox = qobject_cast<QSpinBox *>(ui->list->cellWidget(row, 2));

        // 提取商品名称、价格和数量
        std::string name = nameItem ? nameItem->text().toStdString() : "";
        double price = priceItem ? priceItem->text().toDouble() : 0.00;
        int pPrice = price*100;
        int quantity = quantitySpinBox ? quantitySpinBox->value() : 0;

        InventoryMapper mapperInventory;
        InventoryItemMapper mapperInventoryItem;
        ShoppingListItemMapper mapperShoppingListItem;
        ShoppingListMapper mapperShoppingList;

        Inventory InventoryObject=mapperInventory.getByName(QString::fromStdString(name));
        long long idInventory = InventoryObject.getId();

        mapperInventoryItem.updateByInventoryId(idInventory,pPrice);

        InventoryItem InventoryItemObject = mapperInventoryItem.getByInventory(idInventory);
        long long idInventoryItem = InventoryItemObject.getId();

        ShoppingListItem ShoppingListItemObject(ShoppingListId,idInventoryItem,pPrice,quantity);
    }
}

QPair<QString, QString> SaleInterface::separateValueAndUnit(const QString &str)
{
    // 使用正则表达式来匹配货币符号后跟数字（假设value只包含数字）和斜杠后的unit
    QRegularExpression re("^¥(\\d+)/([^/]+)$");
    QRegularExpressionMatch match = re.match(str);

    if (match.hasMatch()) {
        // 提取value和unit
        QString value = match.captured(1); // 第一个捕获组是value
        QString unit = match.captured(2);  // 第二个捕获组是unit
        return {value, unit};
    } else {
        // 如果格式不匹配，返回空字符串对
        return {QString(), QString()};
    }
}


void SaleInterface::on_cashPay_clicked()
{
        // 获取当前的商品总价
        double totalPrice = this->totalPrice; // 如果 totalPrice 是 SaleInterface 类的成员变量

        // 创建现金支付对话框对象
        CashPaymentDialog *cashDialog = new CashPaymentDialog(this);

        // 设置商品总价
        cashDialog->setTotalAmount(totalPrice);

        // 连接支付确认信号
        connect(cashDialog, &CashPaymentDialog::paymentConfirmed, this, [=](int change) {
            // 处理支付完成后的逻辑，例如更新库存或者打印小票等
            qDebug() << "Payment confirmed. Change: " << change;
        });

        // 显示现金支付对话框
        cashDialog->exec();

        // 删除现金支付对话框对象
        delete cashDialog;

}

