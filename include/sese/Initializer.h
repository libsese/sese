#pragma once
#include "sese/Config.h"
#include <stack>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    class API InitiateTask {
    public:
        using Ptr = std::shared_ptr<InitiateTask>;

        explicit InitiateTask(std::string name);

        virtual int32_t init() noexcept = 0;
        virtual int32_t destroy() noexcept { return 0; };

        [[nodiscard]] const std::string &getName() const;

    private:
        std::string name;
    };

    class API Initializer {
    public:
        Initializer();
        ~Initializer();

        static void addTask(InitiateTask::Ptr &&task) noexcept;

    private:
        void loadTask(InitiateTask::Ptr &&task) noexcept;
        void unloadTask(InitiateTask::Ptr &task) noexcept;

        std::stack<InitiateTask::Ptr> tasks;
    };
}// namespace sese