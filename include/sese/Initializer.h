/**
 * @file Initializer.h
 * @author kaoru
 * @date 2022年4月24日
 * @brief 初始化器
 */

#pragma once
#include "sese/Config.h"
#include <stack>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    class API InitiateTask {
    public:
        using Ptr = std::unique_ptr<InitiateTask>;

        InitiateTask(InitiateTask &&initiateTask) = default;
        explicit InitiateTask(std::string name);
        virtual ~InitiateTask() = default;

        virtual int32_t init() noexcept = 0;
        virtual int32_t destroy() noexcept { return 0; };

        [[nodiscard]] const std::string &getName() const;

    private:
        std::string name;
    };

    class Initializer {
    public:
        Initializer();
        virtual ~Initializer();

        API static void addTask(InitiateTask::Ptr task) noexcept;

    private:
        void buildInLoadTask(InitiateTask::Ptr &&task) noexcept;
        void buildInUnloadTask(const InitiateTask::Ptr &task) noexcept;

        std::stack<InitiateTask::Ptr> tasks;
    };
}// namespace sese