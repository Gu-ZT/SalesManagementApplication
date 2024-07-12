#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "DataEntity.h"
#include "SaleInterface.h"

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale: uiLanguages) {
        const QString baseName = "Application_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            QApplication::installTranslator(&translator);
            break;
        }
    }

    // 初始化数据库
    Database::init();

    InventoryTypeMapper mapper;
    mapper.createTable();
    InventoryMapper invMapper;
    invMapper.createTable();
    InventoryItemMapper itemMapper;
    itemMapper.createTable();
    ShoppingListMapper listMapper;
    listMapper.createTable();
    ShoppingListItemMapper listItemMapper;
    listItemMapper.createTable();
    ReturnOfGoodsItemMapper goodsMapper;
    goodsMapper.createTable();


    SaleInterface window;
    // window.show();
    // 窗口最大化
    window.showMaximized();
    // 窗口全屏
    // window.showFullScreen();

    return QApplication::exec();
}
