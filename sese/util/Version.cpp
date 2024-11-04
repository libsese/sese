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

#include <sese/util/Version.h>

#include <algorithm>
#include <vector>

using sese::Version;

Version::Version(uint16_t major, uint16_t minor, uint16_t patch, uint16_t revision) : major_(major), minor_(minor), patch_(patch), revision_(revision) {}

std::optional<Version> Version::parse(const std::string &version) {
    const char *start_ptr = version.c_str();
    auto end_ptr = const_cast<char *>(start_ptr);

    const auto MAJOR = static_cast<uint16_t>(std::strtoul(start_ptr, &end_ptr, 10));
    if (*end_ptr == 0) {
        return Version(MAJOR, 0, 0, 0);
    }
    if (*end_ptr != '.') {
        return std::nullopt;
    }

    start_ptr = end_ptr + 1;
    const auto MINOR = static_cast<uint16_t>(std::strtoul(start_ptr, &end_ptr, 10));
    if (*end_ptr == 0) {
        return Version(MAJOR, MINOR, 0, 0);
    }
    if (*end_ptr != '.') {
        return std::nullopt;
    }

    start_ptr = end_ptr + 1;
    const auto PATCH = static_cast<uint16_t>(std::strtoul(start_ptr, &end_ptr, 10));
    if (*end_ptr == 0) {
        return Version(MAJOR, MINOR, PATCH, 0);
    }
    if (*end_ptr != '.') {
        return std::nullopt;
    }

    start_ptr = end_ptr + 1;
    const auto REVISION = static_cast<uint16_t>(std::strtoul(start_ptr, &end_ptr, 10));
    if (*end_ptr == 0) {
        return Version(MAJOR, MINOR, PATCH, REVISION);
    }

    return std::nullopt;
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

    if (patch_ || count) {
        vector.emplace_back(patch_);
        count++;
    }

    if (minor_ || count) {
        vector.emplace_back(minor_);
        count++;
    }

    if (major_ || count) {
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