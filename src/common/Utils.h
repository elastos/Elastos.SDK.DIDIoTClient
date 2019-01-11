
#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

namespace elastos {

#define TEST_NET    false

#define TEST_DB_FILE    "ElastosWalletTest.db"
#define DB_FILE         "ElastosWallet.db"

class Utils {

public:
    static std::string Hex2Str(const uint8_t* buff, int len);

    static int Str2Hex(const std::string& str, uint8_t** buf);

};

} // namespace elastos

#endif //__UTILS_H__
