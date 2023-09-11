/**
 * @file ConfigUtil.h
 * @brief 传统配置文件解析工具
 * @author kaoru
 * @date 2023年6月17日
 * @version 0.1.0
 */

#pragma once

#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"

#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /**
     * @brief 传统配置文件类
     */
    class API ConfigObject {
    public:
        using Ptr = std::shared_ptr<ConfigObject>;

        /// 节
        struct API Section {
            using Ptr = std::shared_ptr<Section>;

            std::string name;
            std::map<std::string, std::string> parameter{};

            explicit Section(std::string name);
            /**
             * 根据键名称获取值
             * @param key 键名称
             * @param defaultValue 默认值
             * @return 键存在时返回对应的值，不存在时返回默认值
             */
            std::string getValueByKey(const std::string &key, const std::string &defaultValue);
            void setKeyValue(const std::string &key, const std::string &value);
        };

    public:
        ConfigObject();
        [[nodiscard]] Section::Ptr getDefaultSection() const noexcept { return this->defaultSection; }
        /**
         * 根据节名称获取节
         * @param sectionName 节的名称
         * @return 节存在返回节指针，不存在返回 nullptr
         */
        Section::Ptr getSectionByName(const std::string &sectionName);
        void setSection(const Section::Ptr &section);
        [[nodiscard]] const std::map<std::string, Section::Ptr> &getAllSection() const;

    private:
        Section::Ptr defaultSection;
        std::map<std::string, Section::Ptr> sections;
    };

    /**
     * @brief 传统配置文件工具
     */
    class API ConfigUtil {
    public:
        using InputStream = io::InputStream;
        using OutputStream = io::OutputStream;

        /**
         * 读取配置文件
         * @param fileName 文件名称
         * @return 配置文件类指针，读取失败返回 nullptr
         */
        static ConfigObject::Ptr readFrom(InputStream *input);
        /**
         * 写入配置文件
         * @param configFile 具体的配置
         * @param fileName 欲写入的配置文件名称
         * @return 是否写入成功
         */
        static bool write2(const ConfigObject::Ptr &configFile, OutputStream *output);

    private:
        static std::string readLine(InputStream *input);
    };
}// namespace sese