#include "Error.h"

void error(int pErrorCode, const char *msg) {
    std::cout << msg << ", error code:" << pErrorCode << "\n";
    exit(pErrorCode);
}

void error(int pErrorCode) {
    error(pErrorCode, "Program error");
}
