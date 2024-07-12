#include <QMessageBox>
#include <QStringListModel>
#include "../ui_EnterInterface.h"
#include "EnterInterface.h"

EnterInterface::EnterInterface(QWidget *parent) :
        QWidget(parent), AbstractInterface(), ui(new Ui::EnterInterface) {
    // 初始化保存ID和库存ID为-1
    this->saveId = -1;
    this->inventoryId = -1;
    this->searchList = {};
    this->typeList = {};

    // 设置用户界面布局
    this->ui->setupUi(this);
    this->flush();
    this->ui->searchBox->selectAll();

    // 初始化类型添加接口，并设置主窗口指针
    this->typeAddInterface = new TypeAddInterface;
    this->typeAddInterface->setMainWindow(this);

    // 注册监听"flush"事件，当收到"flush"信号时执行flush方法
    this->listen("flush", [this] {
        this->flush();
    });

    // 连接搜索框回车键按下事件到onBarcodeScanned方法
    connect(this->ui->searchBox, &QLineEdit::returnPressed, this, &EnterInterface::onBarcodeScanned);


    connect(this->ui->listView_2, &QListView::doubleClicked, this, &EnterInterface::setData);

    // 连接返回按钮点击事件到返回逻辑处理
    connect(this->ui->backBtn, &QPushButton::clicked, this, [this] {
        this->getMainWindow()->showMaximized();
        this->clear();
        this->close();
    });

    // 连接类型添加按钮点击事件到类型添加界面显示逻辑
    connect(this->ui->typeAddBtn, &QPushButton::clicked, this, [this] {
        this->typeAddInterface->show();
    });

    // 连接确定按钮点击事件到保存逻辑处理
    connect(this->ui->determine, &QPushButton::clicked, this, [this] {
        this->save();
    });

    // 连接减少数量按钮点击事件到减少数量逻辑
    connect(this->ui->dwindleNumbers, &QPushButton::clicked, this, [this] {
        this->ui->number->setValue(this->ui->number->value() - 10);
    });

    // 连接增加数量按钮点击事件到增加数量逻辑
    connect(this->ui->increaseQuantity, &QPushButton::clicked, this, [this] {
        this->ui->number->setValue(this->ui->number->value() + 10);
    });

    // 连接销售截止日期选择事件到过期日期更新逻辑
    connect(this->ui->sellByDate, &QSpinBox::valueChanged, [this](int value) {
        this->ui->expiringDate->setDate(this->ui->dateOfManufacture->date().addMonths(value));
    });
}


EnterInterface::~EnterInterface() {
    delete this->ui;
}

//处理输入条码
void EnterInterface::onBarcodeScanned() {
    QString barcode = this->ui->searchBox->text().trimmed(); // 获取扫描到的条形码并去除两端空白

    if (!barcode.isEmpty()) {
        // 将文本框中的内容全部选中，以便下次扫码时覆盖旧条码
        this->ui->searchBox->selectAll();

        // 处理条码数据
        processBarcode(barcode);
    }
}

/**
 * 设置类型列表，更新界面显示
 *
 * 本函数用于更新界面中类型列表的显示，首先清空当前的类型列表和界面显示，
 * 然后遍历传入的类型列表，将非空的类型添加到内部存储的类型列表中，并在界面中添加相应的条目。
 * 这样做可以确保界面始终显示最新的类型信息，同时避免显示空类型项。
 *
 * @param pTypeList 一个包含待显示类型信息的列表。
 */
void EnterInterface::setTypeList(QList<InventoryType> pTypeList) {
    // 清空当前的类型列表，为更新做准备
    this->typeList.clear();
    // 清空界面的类型项，准备添加新的类型项
    this->ui->category->clear();

    // 遍历传入的类型列表
    for (InventoryType &item: pTypeList) {
        // 如果当前类型项为空，则跳过，不进行添加
        if (item.isEmpty()) continue;
        // 将非空的类型项添加到内部存储的类型列表中
        this->typeList.append(item);
        // 将类型项的名称转换为字符串并添加到界面的类型列表中
        this->ui->category->addItem(QString::fromStdString(item.getName()));
    }
}


/**
 * 清空并更新物品类型列表。
 *
 * 该方法首先清空当前的物品类型列表，然后基于一个新的InventoryTypeMapper对象
 * 来获取最新的物品类型列表，并将其设置为当前的物品类型列表。这样做可以确保
 * 界面显示的物品类型列表总是最新的，避免了因旧数据导致的显示错误。
 *
 * 注意：这里使用了动态内存分配来创建InventoryTypeMapper对象，因此在使用完
 * 后需要通过delete释放内存，以避免内存泄漏。
 */
void EnterInterface::flush() {
    // 清空当前的物品类型列表
    this->typeList = {};

    // 创建一个新的InventoryTypeMapper对象来获取最新的物品类型列表
    InventoryTypeMapper *mapper = new InventoryTypeMapper;

    // 使用新的物品类型列表更新当前的物品类型列表
    this->setTypeList(mapper->getList());

    // 释放InventoryTypeMapper对象所占用的内存
    delete mapper;
}


/**
 * 处理条形码输入，根据条形码的不同情况执行相应的操作。
 * 当前界面的条形码文本框与输入的条形码相同时，数量加1；否则，根据条形码查询数据库，
 * 如果条形码不存在，则在界面显示新条形码和数量1；如果条形码存在，则更新界面显示
 * 该条形码对应的商品信息，并将数量设置为该商品的入库数量加1。
 *
 * @param barcode 输入的条形码字符串。
 */
void EnterInterface::processBarcode(const QString &barcode) {
    static QRegularExpression re("^[0-9]{13}$");
    if (!re.match(barcode).hasMatch()) {
        this->fuzzySearch(barcode);
        return;
    }
    // 检查当前文本框中的条形码是否与输入的条形码相同
    if (this->ui->barCode->text() == barcode) {
        // 如果相同，数量加1
        this->ui->number->setValue(this->ui->number->value() + 1);
    } else {
        // 如果不同，先保存当前状态
        this->save();
        // 创建映射器，查询数据库中与输入条形码对应的商品信息
        InventoryItemMapper mapper;
        QList<InventoryItem> list = mapper.getByCode(barcode);
        // 检查查询结果是否为空
        if (list.isEmpty()) {
            // 如果为空，说明是新条形码，更新界面显示
            this->ui->barCode->setText(barcode);
            this->ui->number->setValue(1);
        } else {
            // 如果不为空，取第一个商品信息进行处理
            InventoryItem item = list.first();
            // 保存商品ID，用于后续操作
            this->saveId = item.getId();
            // 创建库存映射器，查询库存信息
            InventoryMapper inventoryMapper;
            this->inventoryId = item.getInventory();
            Inventory inventory = inventoryMapper.getById(this->inventoryId);
            // 更新界面显示的商品信息
            this->ui->barCode->setText(QString::fromStdString(item.getCode()));
            this->ui->productName->setText(QString::fromStdString(inventory.getName()));
            this->ui->purchasePrice->setValue(item.getPurchase() / 100.0);
            this->ui->productPrice->setValue(item.getPrice() / 100.0);
            this->ui->specifications->setText(QString::fromStdString(inventory.getSpec()));
            this->ui->dateOfManufacture->setDate(QDateTime::fromMSecsSinceEpoch(item.getManufacture()).date());
            this->ui->expiringDate->setDate(QDateTime::fromMSecsSinceEpoch(item.getShelfLife()).date());
            // 根据商品类型，更新界面显示的类别选项
            for (int i = 0; i < this->typeList.size(); i++) {
                if (this->typeList.at(i).getId() == inventory.getType()) {
                    this->ui->category->setCurrentIndex(i);
                    break;
                }
            }
            // 设置数量为商品入库数量加1
            this->ui->number->setValue(item.getIncomingAmount() + 1);
        }
    }
}


/**
 * 保存商品信息。
 *
 * 该方法通过收集用户界面中的输入数据，创建并保存一个库存物品或库存物品项。
 * 如果是新物品，则首先创建并保存物品信息，然后创建并保存物品项。
 * 如果是已有物品项的更新，则直接更新该物品项。
 * 在任何一步中，如果数据不合法，则方法将终止执行并返回。
 */
void EnterInterface::save() {
    // 获取条形码字符串，如果为空则直接返回。
    std::string barcode = this->ui->barCode->text().toStdString();
    if (barcode.empty()) return;

    // 获取商品名称字符串，如果为空则直接返回。
    std::string name = this->ui->productName->text().toStdString();
    if (name.empty()) return;

    // 获取采购价格，转换为分，并检查是否合法。
    int purchasePrice = (int) floor(this->ui->purchasePrice->value() * 100);
    if (purchasePrice <= 0) return;

    // 获取销售价格，转换为分，并检查是否合法。
    int productPrice = (int) floor(this->ui->productPrice->value() * 100);
    if (productPrice <= 0) return;

    // 获取规格字符串，如果为空则直接返回。
    std::string specifications = this->ui->specifications->text().toStdString();
    if (specifications.empty()) return;

    // 获取生产日期的时间戳，如果非法则直接返回。
    long long dateOfManufacture = this->ui->dateOfManufacture->dateTime().toMSecsSinceEpoch();
    if (dateOfManufacture <= 0) return;

    // 获取保质期的时间戳，如果非法则直接返回。
    long long expiringDate = this->ui->expiringDate->dateTime().toMSecsSinceEpoch();
    if (expiringDate <= 0) return;

    // 获取类别ID，如果非法则直接返回。
    long long category = this->typeList.value(this->ui->category->currentIndex(), InventoryTypeEmpty).getId();
    if (category == InventoryTypeEmpty.getId() || category < 0) return;

    // 获取商品数量，如果小于等于0则直接返回。
    int number = this->ui->number->value();
    if (number <= 0) return;

    // 如果是新物品，则创建并保存物品信息。
    if (this->inventoryId == -1) {
        InventoryMapper mapper;
        Inventory inventory(name, specifications);
        inventory.setType(category);
        mapper.save(inventory);
        this->inventoryId = inventory.getId();
    }

    // 创建库存物品项，并根据是否有ID来决定是保存还是更新该物品项。
    InventoryItemMapper mapper;
    InventoryItem item(this->inventoryId, barcode, dateOfManufacture, expiringDate, Time::current(), number, 0, 0,
                       purchasePrice, productPrice);
    if (this->saveId != -1) {
        item.setId(this->saveId);
        mapper.update(item);
    } else {
        mapper.save(item);
    }

    // 重置界面，准备新的输入。
    this->initInterface();
}


/**
 * 清空界面输入字段并重置相关属性。
 * 该方法用于界面的初始化或重置操作，确保所有与产品相关的信息字段被清空，
 * 并恢复到默认状态。这包括搜索框、条形码、产品名称、采购价格、产品价格、
 * 规格、生产日期、最佳销售日期、过期日期、类别和数量等字段。此外，还重置了
 * 库存ID和保存ID为-1，表示没有选择特定的库存项或保存项。
 */
void EnterInterface::clear() {
    this->ui->searchBox->clear();
    this->ui->barCode->clear();
    this->ui->productName->clear();
    this->ui->purchasePrice->setValue(0);
    this->ui->productPrice->setValue(0);
    this->ui->specifications->clear();
    this->ui->dateOfManufacture->setDate(QDate::currentDate());
    this->ui->sellByDate->setValue(0);
    this->ui->expiringDate->setDate(QDate::currentDate());
    this->ui->category->setCurrentIndex(0);
    this->ui->number->setValue(0);
    this->inventoryId = -1;
    this->saveId = -1;
}


/**
 * 初始化界面的方法
 *
 * 该方法用于在进入界面时进行初始化操作，主要包括以下几个步骤：
 * 1. 清除界面中可能存在的上一次操作的数据或状态。
 * 2. 刷新界面，确保界面显示的是最新的状态。
 * 3. 将焦点设置在搜索框上，使得用户可以立即开始输入。
 * 4. 选中搜索框中的所有内容，方便用户快速替换或清除之前的搜索关键字。
 *
 * 这些操作的目的是为了提供一个干净、立即可用的搜索界面，提高用户的操作效率。
 */
void EnterInterface::initInterface() {
    this->clear();                  // 清除界面状态
    this->flush();                  // 刷新界面
    this->ui->searchBox->setFocus();    // 设置搜索框获得焦点
    this->ui->searchBox->selectAll();  // 选中搜索框中的所有内容
}

/**
 * 实现模糊搜索功能。
 * 根据关键字搜索库存物品和相关物品信息，并在界面上显示搜索结果。
 *
 * @param keyWord 搜索关键字。
 */
void EnterInterface::fuzzySearch(const QString &keyWord) {
    // 清空之前的搜索结果
    this->searchList.clear();
    // 如果关键字为空，则直接返回，不进行搜索
    if (keyWord.isEmpty()) {
        return;
    }
    // 初始化物品映射器，用于根据关键字搜索物品
    InventoryItemMapper mapper;
    // 将搜索到的物品添加到搜索结果列表中
    this->searchList.append(mapper.search(keyWord));
    // 初始化库存映射器，用于根据关键字搜索库存信息
    InventoryMapper inventoryMapper;
    // 根据关键字搜索库存信息
    QList<Inventory> inventories = inventoryMapper.search(keyWord.toStdString());
    // 遍历搜索到的库存信息，查找对应的物品信息，并添加到搜索结果中
    for (auto &item: inventories) {
        // 如果库存信息为空，则跳过
        if (item.isEmpty()) continue;
        // 根据库存信息的ID，查找对应的物品信息
        this->searchList.append(mapper.getItemsByInventory(item.getId()));
    }
    // 初始化一个列表，用于存储最终的搜索结果字符串
    QList<QString> list = {};
    // 遍历搜索结果列表，组合物品代码和名称，形成最终的搜索结果字符串
    for (auto &item: this->searchList) {
        // 如果物品信息为空，则跳过
        if (item.isEmpty()) continue;
        // 根据物品ID，获取物品的名称
        std::string name = inventoryMapper.getById(item.getInventory()).getName();
        // 将物品代码和名称组合成字符串，添加到结果列表中
        list.append(QString::fromStdString(item.getCode() + "|" + name));
    }
    // 更新界面的列表视图，显示搜索结果
    this->ui->listView_2->setModel(new QStringListModel(list));
}


/**
 * 设置界面数据，根据给定的索引从搜索列表中获取物品信息，并显示在界面相应的位置。
 *
 * @param index QModelIndex对象，用于定位搜索列表中的物品。
 */
void EnterInterface::setData(const QModelIndex &index) {
    // 如果索引行号为-1，则直接返回，不做任何操作
    if (index.row() == -1) return;

    // 如果搜索列表为空，则直接返回，不做任何操作
    if (this->searchList.isEmpty()) return;

    // 如果索引超出搜索列表的范围，则直接返回，不做任何操作
    if (this->searchList.size() <= index.row()) return;

    // 获取搜索列表中指定行的物品信息
    InventoryItem item = this->searchList.at(index.row());

    // 如果物品信息为空，则直接返回，不做任何操作
    if (item.isEmpty()) return;

    // 创建一个InventoryMapper对象
    InventoryMapper mapper;
    // 根据物品信息中的库存ID，获取对应的库存信息
    Inventory inventory = mapper.getById(item.getInventory());

    // 如果库存信息为空，则直接返回，不做任何操作
    if (inventory.isEmpty()) return;

    // 根据库存信息，设置产品名称和规格文本框的文本
    this->ui->productName->setText(QString::fromStdString(inventory.getName()));
    this->ui->specifications->setText(QString::fromStdString(inventory.getSpec()));

    // 如果物品的采购价格大于0，则设置采购价格文本框的值
    if (item.getPurchase() > 0) {
        this->ui->purchasePrice->setValue(item.getPurchase() / 100.0);
    }

    // 如果物品的销售价格大于0，则设置销售价格文本框的值
    if (item.getPrice() > 0) {
        this->ui->productPrice->setValue(item.getPrice() / 100.0);
    }

    // 如果库存的类型大于0，则根据类型ID在类型列表中找到对应的类型，并设置分类下拉框的当前索引
    if (inventory.getType() > 0) {
        for (int i = 0; i < this->typeList.size(); i++) {
            if (this->typeList.at(i).getId() == inventory.getType()) {
                // 如果找到了对应的类型，则设置分类下拉框的当前索引
                this->ui->category->setCurrentIndex(i);
                break;
            }
        }
    }
}

