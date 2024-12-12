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

/// \file Path.h
/// \brief UNIX-LIKE Path Conversion Class
/// \author kaoru
/// \date October 26, 2023

#pragma once

#include <sese/Config.h>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese::system {

/// UNIX-LIKE Path Conversion Class
class Path {
public:
    /** \brief Path constructor
     *  \verbatim
for Windows:
    /c/Windows -> c:/Windows
    /C/Users   -> C:/Users
    ./Dir      -> ./Dir
    Dir        -> Dir
     *  \endverbatim
     *  \param unix_path UNIX-LIKE format path
     */
    Path(const char *unix_path) noexcept; // NOLINT

    /** \brief Path constructor
     *  \verbatim
for Windows:
    /c/Windows -> c:/Windows
    /C/Users   -> C:/Users
    ./Dir      -> ./Dir
    Dir        -> Dir
     *  \endverbatim
     *  \param unix_path UNIX-LIKE format path
     */
    Path(const std::string_view &unix_path) noexcept; // NOLINT

    Path() = default;

    /// Convert native path to UNIX-LIKE path
    /// \warning No extra checks
    /// \param native_path Native path
    /// \return UNIX-LIKE path
    static Path fromNativePath(const std::string &native_path) noexcept;

#ifdef SESE_PLATFORM_WINDOWS
    static void replaceWindowsPathSplitChar(char *path, size_t len) noexcept;

    [[nodiscard]] const std::string &getNativePath() const { return native_path; }
#else
    [[nodiscard]] const std::string &getNativePath() const { return unix_path; }
#endif

    [[nodiscard]] const std::string &getUnixPath() const { return unix_path; }

    /// \return Whether the current path is valid
    [[nodiscard]] bool isValid() const { return valid; }

protected:
#ifdef SESE_PLATFORM_WINDOWS
    std::string native_path{};
#endif
    std::string unix_path{};

    bool valid = false;
};

} // namespace sese::system
