/**
 * \file SharedMemory.h
 * \author kaoru
 * \date 2022.12.08
 * \brief 共享内存类
 * \version 0.1
 */
#pragma once

#include <sese/Config.h>
#ifdef WIN32
#else
#include <sys/shm.h>
#endif

namespace sese::system {

    /// \brief 共享内存类
    class API SharedMemory final {
    public:
        using Ptr = std::unique_ptr<SharedMemory>;

        /// 创建一块共享内存
        /// \param name 共享内存名称
        /// \param size 共享内存大小
        /// \return 共享内存对象指针
        /// \retval nullptr 创建失败
        static SharedMemory::Ptr create(const char *name, size_t size) noexcept;
        /// 使用一块现有的共享内存
        /// \param name 共享内存名称
        /// \return 共享内存对象指针
        /// \retval nullptr 获取失败
        static SharedMemory::Ptr use(const char *name) noexcept;
        /// 析构函数
        ~SharedMemory() noexcept;
        /// 获取共享内存实际地址
        /// \return 共享内存块
        void *getBuffer() noexcept;

    private:
        SharedMemory() = default;

    private:
#ifdef WIN32
        void *hMapFile = nullptr;
#else
        int id{};
        static key_t name2key(const char *name) noexcept;
#endif
        void *buffer = nullptr;
        bool isOwner{};
    };
}// namespace sese