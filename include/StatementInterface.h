#ifndef STATEMENT_H
#define STATEMENT_H

#include <QMainWindow>
#include <QtCharts>

#include "AbstractInterface.h"


QT_BEGIN_NAMESPACE
namespace Ui { class StatementInterface; }
QT_END_NAMESPACE

class StatementInterface : public QMainWindow, public AbstractInterface {
Q_OBJECT

public:
    StatementInterface(QWidget *parent = nullptr);

    ~StatementInterface();

    void initChart();

private slots:

    //点击
    static void onPieSeriesClicked(QPieSlice *);

private:
    Ui::StatementInterface *ui;

    //饼状图
    QPieSeries *pie_series{};
};

#endif // STATEMENT_H
