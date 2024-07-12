#include "../ui_StatementInterface.h"
#include "StatementInterface.h"

#include <QVBoxLayout>

StatementInterface::StatementInterface(QWidget *parent)
        : QMainWindow(parent), AbstractInterface(), ui(new Ui::StatementInterface) {
    ui->setupUi(this);
    ui->profitView->chart();

    connect(ui->backBtn, &QPushButton::clicked, this, [this] {
        this->getMainWindow()->showMaximized();
        this->close();
    });

    initChart();
}

StatementInterface::~StatementInterface() {
    delete ui;
}

void StatementInterface::initChart() {
    // 饼状图
    pie_series = new QPieSeries(this);
    connect(pie_series, SIGNAL(clicked(QPieSlice * )), this, SLOT(onPieSeriesClicked(QPieSlice * )));// clazy:exclude=connect-not-normalized

    // 定义各扇形切片的颜色
    static const QStringList list_pie_color = {
            "#6480D6", "#A1DC85", "#FFAD25", "#FF7777", "#84D1EF", "#4CB383",
    };

    // 设置数据
    QList<qreal> list_data = {3.1,3.4 , 3.3, 3.4, 3.5, 3.6}; // 连接数据库的类别

    // 扇形
    for (int i = 0; i < list_pie_color.size(); i++) {
        QPieSlice *pie_slice = new QPieSlice(this);
        pie_slice->setLabelVisible(true);
        pie_slice->setValue(list_data[i]);
        pie_slice->setLabel(QString::number(list_data[i]));
        pie_slice->setColor(list_pie_color[i]);
        pie_slice->setLabelColor(list_pie_color[i]);
        pie_slice->setBorderColor(list_pie_color[i]);
        pie_series->append(pie_slice);
    }

    // 图表视图
    QChart *chart = new QChart;
    chart->setTitle("该时间段内各类商品利润占比");
    // 设置明亮主题
    chart->setTheme(QChart::ChartThemeLight);

    // 标题字体
    QFont font = qApp->font();
    font.setBold(true);
    font.setPointSize(16);
    chart->setTitleFont(font);

    // 加入饼图
    chart->addSeries(pie_series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 图例
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setBackgroundVisible(false);

    // 创建 QChartView 对象并设置属性
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 将 chartView 设置给 profitView 控件
    ui->profitView->setChart(chart);
}

//点击饼图
void StatementInterface::onPieSeriesClicked(QPieSlice *slice) {
    slice->setExploded(!slice->isExploded());
}
