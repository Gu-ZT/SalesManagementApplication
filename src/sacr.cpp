#include "sacr.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QCoreApplication>

Sacr::Sacr(QWidget *parent) : QDialog(parent), AbstractInterface() {
    setModal(true); // 设置为模态对话框
    imageLabel = new QLabel(this);
    QPixmap pixmap("D:/桌面/微信图片_20240712153644.png"); // 假设图片在资源文件中
    imageLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio)); // 设置图片并缩放

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel);
    setLayout(layout);

    QPushButton *buttonBox = new QPushButton("确定", this);
    buttonBox->setObjectName("buttonBox"); // 设置对象名称以便于在UI设计器中识别

    connect(buttonBox, &QPushButton::clicked, this, &Sacr::onAcceptClicked);
    if (AbstractInterface *interface = dynamic_cast<AbstractInterface *>(this->getMainWindow())) {
        interface->apply("flush");
    }
}

void Sacr::onAcceptClicked() {
    accept(); // 接受对话框，这将关闭对话框并返回到调用者
}
