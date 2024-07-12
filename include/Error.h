#ifndef APPLICATION_ERROR_H
#define APPLICATION_ERROR_H

#include <cstdlib>
#include <iostream>

#define DATABASE_CONNECT_ERROR 503

/**
 * 程序错误
 * @param pErrorCode 错误代码
 * @param msg 错误信息
 *
 * @author Gugle
 */
void error(int pErrorCode, const char *msg);

/**
 * 程序错误
 * @param pErrorCode 错误代码
 *
 * @author Gugle
 */
void error(int pErrorCode);

#endif // APPLICATION_ERROR_H
