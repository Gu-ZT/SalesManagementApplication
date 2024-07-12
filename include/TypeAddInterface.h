#ifndef APPLICATION_TYPE_ADD_INTERFACE_H
#define APPLICATION_TYPE_ADD_INTERFACE_H

#include <QWidget>

#include "AbstractInterface.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TypeAddInterface; }
QT_END_NAMESPACE

class TypeAddInterface : public QWidget, public AbstractInterface {
Q_OBJECT

public:
    explicit TypeAddInterface(QWidget *parent = nullptr);

    ~TypeAddInterface() override;

private:
    Ui::TypeAddInterface *ui;
};


#endif // APPLICATION_TYPE_ADD_INTERFACE_H
