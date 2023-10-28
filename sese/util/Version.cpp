#include <sese/util/Version.h>

#include <algorithm>
#include <vector>

using sese::Version;

Version::Version(uint16_t major, uint16_t minor, uint16_t patch, uint16_t revision) : major_(major), minor_(minor), patch_(patch), revision_(revision) {}

std::optional<Version> Version::parse(const std::string &version) {
    const char *startPtr = version.c_str();
    char *endPtr = const_cast<char *>(startPtr);

    auto major = static_cast<uint16_t>(std::strtoul(startPtr, &endPtr, 10));
    if (*endPtr == 0) {
        return Version(major, 0, 0, 0);
    } else if (*endPtr != '.') {
        return std::nullopt;
    }

    startPtr = endPtr + 1;
    auto minor = static_cast<uint16_t>(std::strtoul(startPtr, &endPtr, 10));
    if (*endPtr == 0) {
        return Version(major, minor, 0, 0);
    } else if (*endPtr != '.') {
        return std::nullopt;
    }

    startPtr = endPtr + 1;
    auto patch = static_cast<uint16_t>(std::strtoul(startPtr, &endPtr, 10));
    if (*endPtr == 0) {
        return Version(major, minor, patch, 0);
    } else if (*endPtr != '.') {
        return std::nullopt;
    }

    startPtr = endPtr + 1;
    auto revision = static_cast<uint16_t>(std::strtoul(startPtr, &endPtr, 10));
    if (*endPtr == 0) {
        return Version(major, minor, patch, revision);
    } else {
        return std::nullopt;
    }
}

std::string Version::toString() const {
    return std::to_string(major_) + "." + std::to_string(minor_) + "." + std::to_string(patch_) + "." + std::to_string(revision_);
}


std::string Version::toShortString() const {
    std::vector<uint16_t> vector;
    vector.reserve(4);

    size_t count = 0;
    if (revision_) {
        vector.emplace_back(revision_);
        count++;
    }

    if (!patch_ && !count) {
    } else {
        vector.emplace_back(patch_);
        count++;
    }

    if (!minor_ && !count) {
    } else {
        vector.emplace_back(minor_);
        count++;
    }

    if (!major_ && !count) {
    } else {
        vector.emplace_back(major_);
        count++;
    }

    if (count == 0) {
        vector.emplace_back(0);
        count += 1;
    } else {
        std::reverse(vector.begin(), vector.end());
    }

    // vector to versioning string
    std::string version;
    for (size_t i = 0; i < count; i++) {
        version += std::to_string(vector[i]);
        if (i != count - 1) {
            version += ".";
        }
    }

    return version;
}

// GCOVR_EXCL_START

bool Version::operator==(const sese::Version &other) const {
    return major_ == other.major_ && minor_ == other.minor_ && patch_ == other.patch_ && revision_ == other.revision_;
}

bool Version::operator!=(const sese::Version &other) const {
    return major_ != other.major_ || minor_ != other.minor_ || patch_ != other.patch_ || revision_ != other.revision_;
}

bool Version::operator<(const sese::Version &other) const {
    return major_ < other.major_ || minor_ < other.minor_ || patch_ < other.patch_ || revision_ < other.revision_;
}

bool Version::operator>(const sese::Version &other) const {
    return major_ > other.major_ || minor_ > other.minor_ || patch_ > other.patch_ || revision_ > other.revision_;
}

bool Version::operator<=(const sese::Version &other) const {
    return major_ <= other.major_ && minor_ <= other.minor_ && patch_ <= other.patch_ && revision_ <= other.revision_;
}

bool Version::operator>=(const sese::Version &other) const {
    return major_ >= other.major_ && minor_ >= other.minor_ && patch_ >= other.patch_ && revision_ >= other.revision_;
}

// GCOVR_EXCL_STOP