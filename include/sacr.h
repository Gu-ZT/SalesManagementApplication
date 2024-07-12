#ifndef SACR_H
#define SACR_H

#include <QDialog>
#include <QLabel>
#include "AbstractInterface.h"


class QPushButton;

class Sacr : public QDialog,AbstractInterface{
    Q_OBJECT

public:
    explicit Sacr(QWidget *parent = nullptr);


private slots:
    void onAcceptClicked(); // 确定按钮点击事件的槽函数
private:
     QLabel *imageLabel; // 用于显示图片的 QLabel
};

#endif // SACR_H
