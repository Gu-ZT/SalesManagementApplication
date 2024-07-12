#ifndef APPLICATION_ENTER_INTERFACE_H
#define APPLICATION_ENTER_INTERFACE_H

#include <QMainWindow>

#include "AbstractInterface.h"
#include "TypeAddInterface.h"
#include "DataEntity.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class EnterInterface;
}
QT_END_NAMESPACE

class EnterInterface : public QWidget, public AbstractInterface {
Q_OBJECT

public:
    /**
     * @brief EnterInterface的构造函数
     *
     * 初始化EnterInterface窗口，设置界面布局，连接信号和槽，以及初始化相关属性。
     * @param parent 父窗口指针，默认为nullptr。
     */
    explicit EnterInterface(QWidget *parent = nullptr);

    ~EnterInterface() override;

    void initInterface() override;

private:
    Ui::EnterInterface *ui;
    TypeAddInterface *typeAddInterface;
    QList<InventoryType> typeList;
    QList<InventoryItem> searchList;
    long long saveId;
    long long inventoryId;

    void setTypeList(QList<InventoryType> pTypeList);

    void flush();

    void onBarcodeScanned();

    void processBarcode(const QString &barcode);

    void save();

    void clear();

    void fuzzySearch(const QString &keyWord);

    void setData(const QModelIndex &index);
};

#endif // APPLICATION_ENTER_INTERFACE_H
