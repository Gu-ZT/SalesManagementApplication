#include "ReturnInterface.h"
#include "../ui_ReturnInterface.h"
#include "DataEntity.h"

ReturnInterface::ReturnInterface(QWidget *parent)
        : QWidget(parent), AbstractInterface(), ui(new Ui::ReturnInterface) {
    ui->setupUi(this);
}

ReturnInterface::~ReturnInterface() {
    delete ui;
}

