/**
 * @file ArgParser.h
 * @brief 命令行参数解析类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "Config.h"
#include <map>
#include <string>
#include <memory>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /**
     * @brief 命令行参数解析类
     */
    class API ArgParser {
    public:
        typedef std::unique_ptr<ArgParser> Ptr;

    public:
        /**
         * 初始化解析器
         * @param argc 参数个数
         * @param argv 实际参数
         */
        ArgParser(int32_t argc, char **argv);
        /**
         * @return 返回整个参数 Map
         */
        [[nodiscard]] const std::map<std::string, std::string> &getKeyValSet() const noexcept;
        /**
         * 根据参数名称获取参数值
         * @param key 参数名称
         * @param defaultValue 参数默认值
         * @return 返回参数值，参数不存在返回默认值
         */
        [[nodiscard]] const std::string &getValueByKey(const std::string &key, const std::string &defaultValue) const noexcept;

    private:
        std::map<std::string, std::string> keyValSet;
    };

}// namespace sese
