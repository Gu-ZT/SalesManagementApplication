create table `type`
(
    `id`          INTEGER           not null  -- 标识符
        constraint inventory_pk primary key,
    `version`     INTEGER default 0 not null, -- 乐观锁
    `create_time` INTEGER           not null, -- 创建时间
    `update_time` INTEGER           not null, -- 更新时间
    `del_flag`    INTEGER default 0 not null, -- 逻辑删除
    `name`        TEXT              not null  -- 种类名称
);

create table `inventory`
(
    `id`          INTEGER           not null  -- 标识符
        constraint inventory_pk primary key,
    `version`     INTEGER default 0 not null, -- 乐观锁
    `create_time` INTEGER           not null, -- 创建时间
    `update_time` INTEGER           not null, -- 更新时间
    `del_flag`    INTEGER default 0 not null, -- 逻辑删除
    `type`        INTEGER           null,     -- 产品种类
    `name`        TEXT              not null, -- 产品名称
    `spec`        TEXT              not null  -- 产品单位
);

create table `inventory_item`
(
    `id`              INTEGER           not null  -- 标识符
        constraint inventory_item_pk primary key,
    `version`         INTEGER default 0 not null, -- 乐观锁
    `create_time`     INTEGER           not null, -- 创建时间
    `update_time`     INTEGER           not null, -- 更新时间
    `del_flag`        INTEGER default 0 not null, -- 逻辑删除
    `inventory`       INTEGER           not null, -- 库存标识符
    `code`            TEXT              not null, -- 库存条码
    `manufacture`     INTEGER           not null, -- 生产日期
    `shelf_life`      INTEGER           not null, -- 保质期至
    `storage_time`    INTEGER           not null, -- 入库日期
    `incoming_amount` INTEGER           not null, -- 入库数量
    `sold_amount`     INTEGER default 0 not null, -- 售出数量
    `storage_amount`  INTEGER           not null, -- 库存数量
    `purchase`        INTEGER           not null, -- 进价
    `price`           INTEGER           not null  -- 价格
);

create table `shopping_list`
(
    `id`           INTEGER           not null  -- 标识符
        constraint shopping_list_pk primary key,
    `version`      INTEGER default 0 not null, -- 乐观锁
    `create_time`  INTEGER           not null, -- 创建时间
    `update_time`  INTEGER           not null, -- 更新时间
    `del_flag`     INTEGER default 0 not null, -- 逻辑删除
    `code`         TEXT              not null, -- 小票号
    `confirm_time` INTEGER           not null, -- 确认时间
    `price`        INTEGER           not null, -- 应付
    `payment`      INTEGER           not null, -- 实付
    `change`       INTEGER           not null  -- 找零
);

create table `shopping_list_item`
(
    `id`             INTEGER           not null  -- 标识符
        constraint shopping_list_item_pk primary key,
    `version`        INTEGER default 0 not null, -- 乐观锁
    `create_time`    INTEGER           not null, -- 创建时间
    `update_time`    INTEGER           not null, -- 更新时间
    `del_flag`       INTEGER default 0 not null, -- 逻辑删除
    `shopping_list`  INTEGER           not null, -- 购物单标识符
    `inventory_item` INTEGER           not null, -- 库存项标识符
    `price`          INTEGER           not null, -- 商品价格
    `amount`         INTEGER           not null  -- 商品数量
);

create table `return_of_goods`
(
    `id`             INTEGER           not null  -- 标识符
        constraint return_of_goods_pk primary key,
    `version`        INTEGER default 0 not null, -- 乐观锁
    `create_time`    INTEGER           not null, -- 创建时间
    `update_time`    INTEGER           not null, -- 更新时间
    `del_flag`       INTEGER default 0 not null, -- 逻辑删除
    `shopping_list`  INTEGER           not null, -- 购物单标识符
    `inventory_item` INTEGER           not null, -- 库存项标识符
    `price`          INTEGER           not null, -- 商品价格
    `amount`         INTEGER           not null  -- 退货数量
);
