#ifndef APPLICATION_DATA_ENTITY_H
#define APPLICATION_DATA_ENTITY_H

#include "Database.h"

/**
 * 数据实体基类
 *
 * @author Gugle
 */
class DataEntity {
private:
    long long id; // 标识符
    int version = 0; // 乐观锁
    long long createTime; // 创建时间
    long long updateTime; // 更新时间
    bool delFlag = false;  // 逻辑删除
public:
    DataEntity();

    /**
     * @return 数据实体 ID
     */
    [[nodiscard]] long long getId() const;

    /**
     * @return 数据实体版本
     */
    [[nodiscard]] int getVersion() const;

    /**
     * @return 数据实体创建时间
     */
    [[nodiscard]] long long getCreateTime() const;

    /**
     * @return 数据实体更新时间
     */
    [[nodiscard]] long long getUpdateTime() const;

    /**
     * @return 数据实体是否删除
     */
    [[nodiscard]] bool isDelete() const;

    /**
     * 设置数据实体 ID
     *
     * @param pId 数据实体 ID
     * @return 数据实体
     */
    DataEntity &setId(long long pId);

    /**
     * 设置数据实体版本
     *
     * @param pId 数据实体版本
     * @return 数据实体
     */
    DataEntity &setVersion(int pVersion);

    /**
     * 设置数据实体创建时间
     *
     * @param pId 数据实体创建时间
     * @return 数据实体
     */
    DataEntity &setCreateTime(long long pCreateTime);

    /**
     * 设置数据实体更新时间
     *
     * @param pId 数据实体更新时间
     * @return 数据实体
     */
    DataEntity &setUpdateTime(long long pUpdateTime);

    /**
     * 设置数据实体是否删除
     *
     * @param pId 数据实体是否删除
     * @return 数据实体
     */
    DataEntity &setDelete(bool pDelFlag);

    /**
     * 判断该实体是否为空
     *
     * @return 该实体是否为空
     */
    virtual bool isEmpty() = 0;
};

/**
 * 库存类型实体类
 *
 * @author Gugle
 */
class InventoryType : public DataEntity {
private:
    std::string name; // 库存类型名称
public:
    explicit InventoryType(std::string pName);

    /**
     * @return 库存类型名称
     */
    [[nodiscard]] std::string getName() const;

    /**
     * 设置库存名称
     *
     * @param pType 库存类型名称
     * @return 库存类型实体
     */
    InventoryType &setName(std::string &pName);

    bool isEmpty() override;
};

/**
 * 空数据，请勿修改
 */
static InventoryType InventoryTypeEmpty = InventoryType("EMPTY");

/**
 * 库存类型表操作类
 *
 * @author Gugle
 */
class InventoryTypeMapper : public DataMapper<InventoryType> {
public:
    const char *getTableName() override;

    bool createTable() override;

    /**
     * 获取库存类型列表
     *
     * @return 库存类型列表
     */
    QStringList getType();

    /**
     * 根据库存类型获取库存类型实体
     * @param pType      库存类型
     * @return           库存类型实体
     */
    InventoryType getByType(const std::string &pType);

    /**
     * 获取库存类型列表
     *
     * @param maxSize    最大数量
     * @return           库存类型列表
     */
    QList<InventoryType> getList(int maxSize = 20);

    InventoryType getById(long long pId) override;

    DataMapper<InventoryType> &save(InventoryType &data) override;

    DataMapper<InventoryType> &update(InventoryType &data) override;

    DataMapper<InventoryType> &del(InventoryType &data) override;
};

/**
 * 库存数据实体类
 *
 * @author Gugle
 */
class Inventory : public DataEntity {
private:
    long long type = -1; // 产品类型
    std::string name; // 产品名称
    std::string spec; // 产品单位
public:

    Inventory(std::string pName, std::string pSpec);

    /**
     * @return 库存类型
     */
    [[nodiscard]] long long getType() const;

    /**
     * @return 库存名称
     */
    [[nodiscard]] std::string getName() const;

    /**
     * @return 库存单位
     */
    [[nodiscard]] std::string getSpec() const;

    /**
     * 设置库存类型
     *
     * @param pType 库存类型
     * @return 库存数据实体
     */
    Inventory &setType(long long &pType);

    /**
     * 设置库存名称
     *
     * @param pType 库存名称
     * @return 库存数据实体
     */
    Inventory &setName(std::string &pName);

    /**
     * 设置库存单位
     *
     * @param pType 库存单位
     * @return 库存数据实体
     */
    Inventory &setSpec(std::string &pSpec);

    bool isEmpty() override;
};

/**
 * 空数据，请勿修改
 */
static Inventory InventoryEmpty = Inventory("EMPTY", "EMPTY");

/**
 * 库存数据表操作类
 *
 * @author Gugle
 */
class InventoryMapper : public DataMapper<Inventory> {
public:
    const char *getTableName() override;

    bool createTable() override;

    QList<Inventory> getByType(long long pType);

    Inventory getByName(const QString &pName);

    QList<Inventory> search(const std::string &pName,int maxSize = 20);

    Inventory getById(long long pId) override;

    QList<Inventory> getList(int maxSize = 20);

    InventoryMapper &save(Inventory &data) override;

    InventoryMapper &update(Inventory &data) override;

    InventoryMapper &del(Inventory &data) override;
};

/**
 * 库存条目数据实体类
 *
 * @author Gugle
 */
class InventoryItem : public DataEntity {
private:
    long long inventory;      //库存标识符
    std::string code;         //条码
    long long manufacture;    //生产日期
    long long shelfLife;      //保质期至
    long long storageTime;    //入库日期
    int incomingAmount;       //入库数量
    int soldAmount;           //售出数量
    int storageAmount;        //库存数量
    int purchase;             //进价
    int price;                //价格
public:
    InventoryItem(long long pInventory, std::string pCode, long long pManufacture, long long pShelfLife,
                  long long pStorageTime, int pIncomingAmount, int pSoldAmount, int pStorageAmount,
                  int pPurchase, int pPrice);

    /**
     * @return 库存实体标识符
     */
    [[nodiscard]] long long getInventory() const;

    /**
     * @return 库存条目条码
     */
    [[nodiscard]] std::string getCode() const;

    /**
     * @return 库存条目生产日期
     */
    [[nodiscard]] long long getManufacture() const;

    /**
     * @return 库存条目保质期至
     */
    [[nodiscard]] long long getShelfLife() const;

    /**
     * @return 库存条目入库时间
     */
    [[nodiscard]] long long getStorageTime() const;

    /**
     * @return 库存条目入库数量
     */
    [[nodiscard]] int getIncomingAmount() const;

    /**
     * @return 库存条目售出数量
     */
    [[nodiscard]] int getSoldAmount() const;

    /**
     * @return 库存条目库存数量
     */
    [[nodiscard]] int getStorageAmount() const;

    /**
     * @return 库存条目进价
     */
    [[nodiscard]] int getPurchase() const;

    /**
     * @return 库存条目价格
     */
    [[nodiscard]] int getPrice() const;

    /**
     * 设置库存实体标识符
     *
     * @param pInventory 库存实体标识符
     * @return 库存条目数据实体
     */
    InventoryItem &setInventory(long long &pInventory);

    /**
     * 设置库存条目条码
     *
     * @param pCode 库存条目条码
     * @return 库存条目数据实体
     */
    InventoryItem &setCode(const std::string &pCode);

    /**
     * 设置库存条目生产日期
     *
     * @param pManufacture 库存条目生产日期
     * @return 库存条目数据实体
     */
    InventoryItem &setManufacture(long long &pManufacture);

    /**
     * 设置库存条目保质期至
     *
     * @param pShelfLife 库存条目保质期至
     * @return 库存条目数据实体
     */
    InventoryItem &setShelfLife(long long &pShelfLife);

    /**
     * 设置库存条目入库时间
     *
     * @param pStorageTime 存条目入库时间
     * @return 库存条目数据实体
     */
    InventoryItem &setStorageTime(long long &pStorageTime);

    /**
     * 设置库存条目入库数量
     *
     * @param pIncomingAmount 库存条目入库数量
     * @return 库存条目数据实体
     */
    InventoryItem &setIncomingAmount(int &pIncomingAmount);

    /**
     * 设置库存条目售出数量
     *
     * @param pSoldAmount 库存条目售出数量
     * @return 库存条目数据实体
     */
    InventoryItem &setSoldAmount(int &pSoldAmount);

    /**
     * 设置库存条目库存数量
     *
     * @param pStorageAmount 库存条目库存数量
     * @return 库存条目数据实体
     */
    InventoryItem &setStorageAmount(int &pStorageAmount);

    /**
     * 设置库存条目价格
     *
     * @param pPrice 库存条目价格
     * @return 库存条目数据实体
     */
    InventoryItem &setPrice(int &pPrice);

    /**
     * 设置库存条目价格
     *
     * @param pPurchase 库存条目价格
     * @return 库存条目数据实体
     */
    InventoryItem &setPurchase(int &pPurchase);

    bool isEmpty() override;
};

/**
 * 空数据，请勿修改
 */
static InventoryItem InventoryItemEmpty = InventoryItem(0, "EMPTY", 0, 0, 0, 0, 0, 0, 0.0, 0.0);

/**
 * 库存条目数据操作类
 *
 * @author Gugle
 */
class InventoryItemMapper : public DataMapper<InventoryItem> {
public:
    const char *getTableName() override;

    bool createTable() override;

    InventoryItem getByInventory(long long pInventory);

    QList<InventoryItem> getItemsByInventory(long long pInventory, int maxSize = 20);

    QList<InventoryItem> getByDate(const QString &startDate, const QString &endDate);

    InventoryItem getById(long long pId) override;

    /**
     * 根据部分条码查询库存条目
     *
     * @param pBarcode 部分条码
     * @param maxSize 最大查询数量
     * @return 查询到的库存条码
     */
    QList<InventoryItem> getByCode(const QString &pBarcode, int maxSize = 20);

    QList<InventoryItem> search(const QString &keyWord, int maxSize = 20);

    InventoryItemMapper &save(InventoryItem &data) override;

    InventoryItemMapper &update(InventoryItem &data) override;

    InventoryItemMapper &updateByInventoryId(long long IdInventory, int quantity);

    InventoryItemMapper &del(InventoryItem &data) override;
};

/**
 * 购物单数据实体类
 *
 * @author Gugle
 */
class ShoppingList : public DataEntity {
private:
    std::string code;      // 小票号
    long long confirmTime; // 确认时间
    int price;             // 应付
    int payment;           // 实付
    int change;            // 找零
public:
    ShoppingList(std::string pCode, long long pConfirmTime, int pPrice, int pPayment, int pChange);

    /**
     * @return  小票号
     */
    [[nodiscard]] std::string getCode() const;

    /**
     * 设置小票号
     *
     * @param pCode 小票号
     * @return 购物单数据实体
     */
    ShoppingList &setCode(std::string pCode);

    /**
     * @return 确认时间
     */
    [[nodiscard]] long long getConfirmTime() const;

    /**
     * 设置确认时间
     *
     * @param pConfirmTime 确认时间
     * @return 购物单数据实体
     */
    ShoppingList &setConfirmTime(long long pConfirmTime);

    /**
     * @return 应付
     */
    [[nodiscard]] int getPrice() const;

    /**
     * 设置应付
     *
     * @param pPrice 应付
     * @return 购物单数据实体
     */
    ShoppingList &setPrice(int pPrice);

    /**
     * @return 实付
     */
    [[nodiscard]] int getPayment() const;

    /**
     * 设置实付
     *
     * @param pPayment 实付
     * @return 购物单数据实体
     */
    ShoppingList &setPayment(int pPayment);

    /**
     * @return 找零
     */
    [[nodiscard]] int getChange() const;

    /**
     * 设置找零
     *
     * @param pChange 找零
     * @return 购物单数据实体
     */
    ShoppingList &setChange(int pChange);

    bool isEmpty() override;
};

/**
 * 空数据，请勿修改
 */
static ShoppingList ShoppingListEmpty = ShoppingList("EMPTY", 0, 0, 0, 0);

/**
 * 购物单数据操作类
 *
 * @author Gugle
 */
class ShoppingListMapper : public DataMapper<ShoppingList> {
public:
    const char *getTableName() override;

    bool createTable() override;

    ShoppingList getById(long long pId) override;

    ShoppingListMapper &save(ShoppingList &data) override;

    ShoppingListMapper &update(ShoppingList &data) override;

    ShoppingListMapper &del(ShoppingList &data) override;
};

/**
 * 购物单项数据实体类
 *
 * @author Gugle
 */
class ShoppingListItem : public DataEntity {
private:
    long long shoppingList;  //购物单标识符
    long long inventoryItem; //库存项标识符
    int price;               //商品价格
    int amount;              //商品数量
public:
    ShoppingListItem(long long pShoppingList, long long pInventoryItem, int pPrice, int pAmount);

    /**
     * @return 购物单标识符
     */
    [[nodiscard]] long long getShoppingList() const;

    /**
     * 设置购物单标识符
     *
     * @param pShoppingList 购物单标识符
     * @return 购物单项数据实体
     */
    ShoppingListItem &setShoppingList(long long pShoppingList);

    /**
     * @return 库存项标识符
     */
    [[nodiscard]] long long getInventoryItem() const;

    /**
     * 设置库存项标识符
     *
     * @param pInventoryItem 库存项标识符
     * @return 购物单项数据实体
     */
    ShoppingListItem &setInventoryItem(long long pInventoryItem);

    /**
     * @return 商品价格
     */
    [[nodiscard]] int getPrice() const;

    /**
     * 设置商品价格
     *
     * @param pPrice 商品价格
     * @return 购物单项数据实体
     */
    ShoppingListItem &setPrice(int pPrice);

    /**
     * @return 商品数量
     */
    [[nodiscard]] int getAmount() const;

    /**
     * 设置商品数量
     *
     * @param pAmount 商品数量
     * @return 购物单项数据实体
     */
    ShoppingListItem &setAmount(int pAmount);

    bool isEmpty() override;
};

/**
 * 空数据，请勿修改
 */
static ShoppingListItem ShoppingListItemEmpty = ShoppingListItem(0, 0, 0, 0);

/**
 * 购物单项数据操作类
 *
 * @author Gugle
 */
class ShoppingListItemMapper : public DataMapper<ShoppingListItem> {
public:
    const char *getTableName() override;

    bool createTable() override;

    ShoppingListItem getById(long long pId) override;

    ShoppingListItemMapper &save(ShoppingListItem &data) override;

    ShoppingListItemMapper &update(ShoppingListItem &data) override;

    ShoppingListItemMapper &del(ShoppingListItem &data) override;
};

/**
 * 退货数据实体类
 *
 * @author Gugle
 */
class ReturnOfGoods : public DataEntity {
private:
    long long shoppingList;  //购物单标识符
    long long inventoryItem; //库存项标识符
    int price;               //商品价格
    int amount;              //退货数量
public:
    ReturnOfGoods(long long pShoppingList, long long pInventoryItem, int pPrice, int pAmount);

    /**
     * @return 购物单标识符
     */
    [[nodiscard]] long long getShoppingList() const;

    /**
     * 设置购物单标识符
     *
     * @param pShoppingList 购物单标识符
     * @return 退货数据实体
     */
    ReturnOfGoods &setShoppingList(long long pShoppingList);

    /**
     * @return 库存项标识符
     */
    [[nodiscard]] long long getInventoryItem() const;

    /**
     * 设置库存项标识符
     *
     * @param pInventoryItem 库存项标识符
     * @return 退货数据实体
     */
    ReturnOfGoods &setInventoryItem(long long pInventoryItem);

    /**
     * @return 商品价格
     */
    [[nodiscard]] int getPrice() const;

    /**
     * 设置商品价格
     *
     * @param pPrice 商品价格
     * @return 退货数据实体
     */
    ReturnOfGoods &setPrice(int pPrice);

    /**
     * @return 退货数量
     */
    [[nodiscard]] int getAmount() const;

    /**
     * 设置退货数量
     *
     * @param pAmount 退货数量
     * @return 退货数据实体
     */
    ReturnOfGoods &setAmount(int pAmount);

    bool isEmpty() override;
};

/**
 * 空数据，请勿修改
 */
static ReturnOfGoods ReturnOfGoodsEmpty = ReturnOfGoods(0, 0, 0, 0);

/**
 * 退货数据操作类
 *
 * @author Gugle
 */
class ReturnOfGoodsItemMapper : public DataMapper<ReturnOfGoods> {
public:
    const char *getTableName() override;

    bool createTable() override;

    ReturnOfGoods getById(long long pId) override;

    ReturnOfGoodsItemMapper &save(ReturnOfGoods &data) override;

    ReturnOfGoodsItemMapper &update(ReturnOfGoods &data) override;

    ReturnOfGoodsItemMapper &del(ReturnOfGoods &data) override;
};

#endif // APPLICATION_DATA_ENTITY_H
