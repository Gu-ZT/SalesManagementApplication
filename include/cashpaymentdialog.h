#ifndef CASHPAYMENTDIALOG_H
#define CASHPAYMENTDIALOG_H

#include <QDialog>

namespace Ui {
class CashPaymentDialog;
}

class CashPaymentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CashPaymentDialog(QWidget *parent = nullptr);
    ~CashPaymentDialog();

    void setTotalAmount(double totalAmount);

signals:
    void paymentConfirmed(int change);

private slots:
    void on_confirmButton_clicked();

private:
    Ui::CashPaymentDialog *ui;
    double totalAmount;
};

#endif // CASHPAYMENTDIALOG_H
