/**
 * @file QueryString.h
 * @author kaoru
 * @brief 查询字符串
 * @date 2022年05月17日
 */

#pragma once

#include <sese/Config.h>
#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net::http {

/**
 * @brief 查询字符串
 */
class API QueryString {
public:
    using Ptr = std::unique_ptr<QueryString>;

    explicit QueryString() noexcept = default;
    explicit QueryString(const std::string &query) noexcept;
    std::string toString() noexcept;

    void set(const std::string &key, const std::string &value) noexcept;
    const std::string &get(const std::string &key, const std::string &defaultValue) noexcept;

    [[nodiscard]] inline size_t size() const noexcept { return keyValueSet.size(); }
    [[nodiscard]] inline bool empty() const noexcept { return keyValueSet.empty(); }
    inline std::map<std::string, std::string>::iterator begin() noexcept { return keyValueSet.begin(); }
    inline std::map<std::string, std::string>::iterator end() noexcept { return keyValueSet.end(); }

protected:
    std::map<std::string, std::string> keyValueSet;
};
} // namespace sese::net::http