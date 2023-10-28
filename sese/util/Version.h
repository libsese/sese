/// \file Version.h
/// \brief 版本类
/// \author kaoru
/// \date 2023年10月28日

#pragma once

#include <sese/Config.h>

#include <optional>

namespace sese {

/// \brief 版本类
class Version {
    uint16_t major_;
    uint16_t minor_;
    uint16_t patch_;
    uint16_t revision_;

public:
    Version(uint16_t major, uint16_t minor, uint16_t patch, uint16_t revision);

    [[nodiscard]] uint16_t getMajor() const { return major_; }
    void setMajor(uint16_t major) { Version::major_ = major; }
    [[nodiscard]] uint16_t getMinor() const { return minor_; }
    void setMinor(uint16_t minor) { Version::minor_ = minor; }
    [[nodiscard]] uint16_t getPatch() const { return patch_; }
    void setPatch(uint16_t patch) { Version::patch_ = patch; }
    [[nodiscard]] uint16_t getRevision() const { return revision_; }
    void setRevision(uint16_t revision) { Version::revision_ = revision; }

    static std::optional<Version> parse(const std::string &version);

    /// 获取完整的版本号字符串
    /// \return 完整的版本号字符串
    [[nodiscard]] std::string toString() const;

    /// 获取简短的版本号字符串，只包含非零部分，例如 "0.2.3"、"1.2"
    /// \return 简短的版本号字符串
    [[nodiscard]] std::string toShortString() const;

    bool operator==(const Version &other) const;
    bool operator<(const Version &other) const;
    bool operator>(const Version &other) const;
    bool operator<=(const Version &other) const;
    bool operator>=(const Version &other) const;
    bool operator!=(const Version &other) const;
};

} // namespace sese
