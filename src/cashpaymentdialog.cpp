#include "../ui_cashpaymentdialog.h"
#include "cashpaymentdialog.h"

CashPaymentDialog::CashPaymentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CashPaymentDialog)
{
    ui->setupUi(this);
}

CashPaymentDialog::~CashPaymentDialog()
{
    delete ui;
}

void CashPaymentDialog::setTotalAmount(double totalAmount)
{
    this->totalAmount = totalAmount;
    ui->TotalAmount->setText(QString::number(totalAmount, 'f', 2));
}

void CashPaymentDialog::on_confirmButton_clicked()
{
    double receivedAmount = ui-> receivedAmount ->text().toDouble();
    int change = receivedAmount - totalAmount;

    // Emit signal with change amount
    emit paymentConfirmed(change);

    // Close the payment dialog
    this->close();
}
