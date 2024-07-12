#include <QMessageBox>
#include "TypeAddInterface.h"
#include "../ui_TypeAddInterface.h"
#include "DataEntity.h"


TypeAddInterface::TypeAddInterface(QWidget *parent)
        : QWidget(parent), AbstractInterface(), ui(new Ui::TypeAddInterface) {
    ui->setupUi(this);
    connect(ui->applyBtn, &QPushButton::clicked, this, [this] {
        if(ui->name->text().isEmpty()) {
            QMessageBox::warning(this, "名称不能为空", "名称不能为空");
            return;
        }
        InventoryType newType(ui->name->text().toStdString());
        InventoryTypeMapper mapper;
        mapper.save(newType);
        // 添加成功提示
        QMessageBox::information(this, "添加成功", "添加成功");
        this->getMainWindow();
        if (AbstractInterface * interface = dynamic_cast<AbstractInterface *>(this->getMainWindow())) {
            interface->apply("flush");
        }
        ui->name->setText("");
        this->close();
    });
}

TypeAddInterface::~TypeAddInterface() {
    delete ui;
}
