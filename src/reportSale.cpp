#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include <QVector>

#include "ui_reportSale.h"

class ReportSale : public QWidget {
    Q_OBJECT

public:
    ReportSale(QWidget *parent = nullptr) : QWidget(parent) {
        ui.setupUi(this);
        connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFirstComboChanged()));
        connect(ui.comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(onSecondComboChanged()));
        populateFirstCombo();
    }

private slots:
    void onFirstComboChanged() {
        populateSecondCombo();
    }

    void onSecondComboChanged() {
        fetchSalesData();
    }

private:
    Ui::Form ui;
    QVector<int> salesData;

    void populateFirstCombo() {
        // 填充第一个下拉框
        ui.comboBox->addItem("商品名");
        ui.comboBox->addItem("类别");
    }

    void populateSecondCombo() {
        // 根据第一个下拉框的选择填充第二个下拉框
        ui.comboBox_2->clear();
        QString selectedOption = ui.comboBox->currentText();

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("/path/to/your/database.sql");

        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", db.lastError().text());
            return;
        }

        QSqlQuery query;
        if (selectedOption == "商品名") {
            query.prepare("SELECT DISTINCT name FROM inventory WHERE del_flag = 0");
        } else if (selectedOption == "类别") {
            query.prepare("SELECT DISTINCT name FROM type WHERE del_flag = 0");
        }
        query.exec();

        while (query.next()) {
            QString item = query.value(0).toString();
            ui.comboBox_2->addItem(item);
        }
    }

    void fetchSalesData() {
        // 根据第二个下拉框的选择查询最近半年每月的销量情况
        QString selectedOption = ui.comboBox->currentText();
        QString selectedItem = ui.comboBox_2->currentText();

        QSqlDatabase db = QSqlDatabase::database();

        QSqlQuery query;
        if (selectedOption == "商品名") {
            query.prepare("SELECT strftime('%Y-%m', datetime(confirm_time, 'unixepoch')) AS month, "
                          "SUM(shopping_list_item.amount) AS total_sales "
                          "FROM shopping_list_item "
                          "JOIN shopping_list ON shopping_list_item.shopping_list = shopping_list.code "
                          "JOIN inventory_item ON shopping_list_item.inventory_item = inventory_item.id "
                          "JOIN inventory ON inventory_item.inventory = inventory.id "
                          "WHERE inventory.name = ? AND confirm_time >= strftime('%s', 'now', '-6 months') "
                          "GROUP BY month ORDER BY month");
            query.addBindValue(selectedItem);
        } else if (selectedOption == "类别") {
            query.prepare("SELECT strftime('%Y-%m', datetime(confirm_time, 'unixepoch')) AS month, "
                          "SUM(shopping_list_item.amount) AS total_sales "
                          "FROM shopping_list_item "
                          "JOIN shopping_list ON shopping_list_item.shopping_list = shopping_list.code "
                          "JOIN inventory_item ON shopping_list_item.inventory_item = inventory_item.id "
                          "JOIN inventory ON inventory_item.inventory = inventory.id "
                          "JOIN type ON inventory.type = type.id "
                          "WHERE type.name = ? AND confirm_time >= strftime('%s', 'now', '-6 months') "
                          "GROUP BY month ORDER BY month");
            query.addBindValue(selectedItem);
        }
        query.exec();

        salesData.clear();
        while (query.next()) {
            int totalSales = query.value(1).toInt();
            salesData.append(totalSales);
        }

        // 显示查询结果
        QString result;
        for (int sale : salesData) {
            result += QString::number(sale) + "\n";
        }
        ui.label->setText(result);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ReportSale reportSale;
    reportSale.show();
    return app.exec();
}

