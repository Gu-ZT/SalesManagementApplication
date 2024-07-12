#ifndef APPLICATION_SALE_INTERFACE_H
#define APPLICATION_SALE_INTERFACE_H

#include <QMainWindow>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "DataEntity.h"
#include "EnterInterface.h"
#include "InventoryInterface.h"
#include "StatementInterface.h"
#include "ReturnInterface.h"
#include "AbstractInterface.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class SaleInterface;
}
QT_END_NAMESPACE

class SaleInterface : public QMainWindow,public AbstractInterface {
Q_OBJECT

public:
    SaleInterface(QMainWindow *parent = nullptr);

    ~SaleInterface();

private slots:

    void updateTotalPrice(); // 更新总价格的槽函数
    void openSacrDialog(); // 打开Sacr对话框的槽函数
    void on_cashPay_clicked(); //打开现金支付的槽函数

private:
    Ui::SaleInterface *ui;
    EnterInterface *enterInterface;
    InventoryInterface *inventoryInterface;
    StatementInterface *statementInterface;
    ReturnInterface *returnInterface;
    double totalPrice; // 所有商品的总价格
    QList<InventoryItem> inventoryItems;

    void onBarcodeScanned();

    void processBarcode(const QString &barcode);//处理条码数据

    void listWidgetDisplay(double price, std::string name, std::string spec);//将信息显示在选择列表上

    void onListViewDoubleClicked(const QModelIndex &index);

    void TableWidget_display(double price, const std::string &name, const std::string &spec);//将信息显示在清单列表上

    double calculateRowTotal(int row); // 计算单行商品的总价

    void pushQuantity();//将清单列表数据存放到数据库

    QPair<QString, QString> separateValueAndUnit(const QString &str);
};

#endif // APPLICATION_SALE_INTERFACE_H
