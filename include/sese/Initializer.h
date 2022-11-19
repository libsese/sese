/**
 * @file Initializer.h
 * @author kaoru
 * @date 2022年4月24日
 * @brief 初始化器
 */

#pragma once
#include <sese/Config.h>
#include <stack>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    class API InitiateTask {
    public:
        /**
         * \brief 初始化任务智能指针
         * \note
         * std::stack 使用 std::deque 实现，
         * 其中 std::deque::top 使用 '=' 运算符，
         * 与 std::unique_ptr 冲突。
         * 所以，InitiateTask::Ptr 使用 std::shared_ptr
         */
        using Ptr = std::shared_ptr<InitiateTask>;

        explicit InitiateTask(std::string name);
        virtual ~InitiateTask() = default;

        virtual int32_t init() noexcept = 0;
        virtual int32_t destroy() noexcept { return 0; };

        [[nodiscard]] const std::string &getName() const;

    private:
        std::string name;
    };

    class API Initializer {
    public:
        Initializer();
        virtual ~Initializer();

    private:
        void buildInLoadTask(InitiateTask::Ptr &&task) noexcept;
        void buildInUnloadTask(const InitiateTask::Ptr &task) noexcept;

        std::stack<InitiateTask::Ptr> tasks;

        // 用户级方法
    public:
        /// 添加任务至初始化器
        /// \param task 初始化任务
        static void addTask(InitiateTask::Ptr task) noexcept;
        /// 获取初始化器指针
        /// \note 将 sese 作为使用静态链接库使用时，请务必使用该函数手动进行初始化
        /// \return 初始化指针，此返回值无用
        [[maybe_unused]] static void *getInitializer() noexcept;
    };

}// namespace sese