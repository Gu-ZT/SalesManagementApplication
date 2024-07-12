#ifndef APPLICATION_DATABASE_H
#define APPLICATION_DATABASE_H

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

#include <utility>
#include <string>
#include <memory>
#include <sstream>

#include "Utils.h"
#include "Error.h"

/**
 * 数据库类
 *
 * @author Gugle
 */
class Database {
private:
    bool inited = false;
    QSqlDatabase database;
public:
    /**
     * @return 数据库是否初始化
     */
    static bool isInited();

    /**
     * 初始化数据库
     *
     * @param path 数据库文件存储地址
     */
    static void init();
};


/**
 * 数据表操作类
 *
 * @tparam T 数据类型
 *
 * @author Gugle
 */
template<typename T>
class DataMapper {
public:
    /**
     * @return 数据表名称
     */
    virtual const char *getTableName() = 0;

    /**
     * 创建数据表
     *
     * @return 是否创建成功
     */
    virtual bool createTable() = 0;

    /**
     * 根据数据实体 ID 获取版本
     *
     * @param pId 数据实体 ID
     * @return 数据版本
     */
    int getVersionById(long long pId);

    /**
     * 根据数据实体 ID 获取数据对象
     *
     * @param pId 数据实体 ID
     * @return 数据对象
     */
    virtual T getById(long long pId) = 0;

    /**
     * 插入新数据
     *
     * @param data 数据
     * @return 数据操作对象
     */
    virtual DataMapper<T> &save(T &data) = 0;

    /**
     * 修改数据
     *
     * @param data 数据
     * @return 数据操作对象
     */
    virtual DataMapper<T> &update(T &data) = 0;

    /**
     * 删除数据
     *
     * @param data 数据
     * @return 数据操作对象
     */
    virtual DataMapper<T> &del(T &data) = 0;
};

#endif // APPLICATION_DATABASE_H
