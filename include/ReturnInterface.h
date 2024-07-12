#ifndef APPLICATION_RETURN_INTERFACE_H
#define APPLICATION_RETURN_INTERFACE_H
#include <QWidget>

#include "AbstractInterface.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ReturnInterface; }
QT_END_NAMESPACE

class ReturnInterface : public QWidget, public AbstractInterface {
Q_OBJECT

public:
    explicit ReturnInterface(QWidget *parent = nullptr);

    ~ReturnInterface() override;

private:
    Ui::ReturnInterface *ui;
};

#endif //APPLICATION_RETURN_INTERFACE_H
