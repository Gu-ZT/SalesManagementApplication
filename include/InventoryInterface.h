#ifndef APPLICATION_INVENTORY_INTERFACE_H
#define APPLICATION_INVENTORY_INTERFACE_H

#include <QWidget>

#include "DataEntity.h"
#include "AbstractInterface.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class InventoryInterface;
}
QT_END_NAMESPACE

class InventoryInterface : public QWidget, public AbstractInterface
{
    Q_OBJECT

public:
    explicit InventoryInterface(QWidget *parent = nullptr);
    ~InventoryInterface() override;
    void addComboBoxItems();

private:
    Ui::InventoryInterface *ui;  // 使用 Ui 命名空间下的 InventoryInterface 类型指针
    void quantityProcess();
    void tableWidgetDisplay(const QString& name,const QString& code,const QString& type,long long storageTime,long long manufacture,long long shelfLife,long long incomingAmount,long long soldAmount,long long storageAmount);
    void quantitydateProcess();
};

#endif // APPLICATION_INVENTORY_INTERFACE_H
