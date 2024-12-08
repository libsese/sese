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
 * @file Initializer.h
 * @author kaoru
 * @date April 24, 2022
 * @brief Initializer
 */

#pragma once

#include "sese/Config.h"
#include <stack>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

/// \brief Initialization task base class
class InitiateTask {
public:
    /**
     * \brief Initialization task smart pointer
     * \note
     * std::stack is implemented using std::deque,
     * and std::deque::top uses the '=' operator,
     * which conflicts with std::unique_ptr.
     * Therefore, InitiateTask::Ptr uses std::shared_ptr.
     */
    using Ptr = std::shared_ptr<InitiateTask>;

    explicit InitiateTask(std::string name);
    virtual ~InitiateTask() = default;

    virtual int32_t init() noexcept = 0;
    virtual int32_t destroy() noexcept = 0;

    [[nodiscard]] const std::string &getName() const;

private:
    std::string name;
};

/// Initializer
class Initializer final { // GCOVR_EXCL_LINE
public:
    ~Initializer();

private:
    std::stack<InitiateTask::Ptr> tasks;

public:
    /// \brief Add a task to the initializer
    /// \param task Initialization task
    static void addTask(const InitiateTask::Ptr &task) noexcept;

    template<class T>
    static void addTask() noexcept {
        Initializer::addTask(std::make_unique<T>());
    }

    /// \brief Get the initializer pointer
    /// \note When using "sese" as a static library, make sure to use this function to initialize manually
    /// \return Initialization pointer, this return value is useless
    /// \deprecated Must manually initialize
    /// \see sese::initCore
    [[maybe_unused]] SESE_DEPRECATED static void *getInitializer() noexcept;
};

/// \brief Initialize sese-core
/// \param argc Number of arguments
/// \param argv Array of arguments
[[maybe_unused]] void initCore(int argc, const char *const *argv) noexcept;

} // namespace sese