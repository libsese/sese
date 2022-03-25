#pragma once
#include "Config.h"
#include <map>
#include <string>
#include <memory>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    class API ArgParser {
    public:
        typedef std::unique_ptr<ArgParser> Ptr;

    public:
        ArgParser(int32_t argc, char **argv);
        [[nodiscard]] const std::map<std::string, std::string> &getKeyValSet() const noexcept;
        [[nodiscard]] const std::string &getValueByKey(const std::string &key, const std::string &defaultValue) const noexcept;

    private:
        std::map<std::string, std::string> keyValSet;
    };

}// namespace sese
