// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file ArgParser.h
 * @brief 命令行参数解析类
 * @author kaoru
 * @date 2023年2月22日
 * @version 0.2
 */
#pragma once

#include "sese/Config.h"

#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

/**
 * @brief 命令行参数解析类
 */
class ArgParser {
public:
    typedef std::unique_ptr<ArgParser> Ptr;

public:
    ArgParser() = default;

    /**
     * 初始化解析器
     * @param argc 参数个数
     * @param argv 实际参数
     * @return 解析是否成功
     */
    bool parse(int32_t argc, const char *const *argv) noexcept;

    /**
     * @return 返回整个参数 Map
     */
    [[nodiscard]] const std::map<std::string, std::string> &getKeyValSet() const noexcept;

    /**
     * 根据参数名称获取参数值
     * @param key 参数名称
     * @param default_value 参数默认值
     * @return 返回参数值，参数不存在返回默认值
     */
    [[nodiscard]] const std::string &getValueByKey(const std::string &key, const std::string &default_value) const noexcept;

    /**
     * Retrieves the value associated with the given key.
     * @param key The name of the parameter.
     * @return The value of the parameter if it exists.
     * @exception std::out_of_range If the key does not exist.
     */
    const std::string &getValueByKey(const std::string &key) const;

    /**
     * 根据参数名称判断当前参数是否存在，适用于一些单独的不需要指定值的开关
     * @param key 参数名称
     * @return 该参数是否存在
     */
    [[nodiscard]] bool exist(const std::string &key) const noexcept;

    /**
     * 获取程序工作目录
     * @return 工作目录字符串，结尾不带目录分割符
     */
    [[nodiscard]] const std::string &getCurrentPath() const { return currentPath; }

    /**
     * 获取程序当前名称
     * @return 程序当前名称
     */
    [[nodiscard]] const std::string &getFileName() const { return fileName; }

    /**
     * 获取程序当前的绝对路径
     * @return 程序路径
     */
    [[nodiscard]] const std::string &getFullPath() const { return fullPath; }

private:
    std::string fullPath;
    std::string currentPath;
    std::string fileName;
    std::map<std::string, std::string> keyValSet;
};

} // namespace sese
