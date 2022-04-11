/**
* @file IOException.h
* @brief IO 异常类
* @author kaoru
* @date 2022年4月11日
*/
#include "Config.h"
#include "Exception.h"

namespace sese {
    /**
     * @brief IO 异常类
     */
    class API IOException : public Exception {
    public:
        explicit IOException(const std::string &str = EXCEPTION_IO_STRING) : Exception(str) {}
    };
}