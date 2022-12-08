#pragma once

#include <sese/Config.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <sys/shm.h>
#endif

namespace sese {

    class API SharedMemory final {
    public:
        using Ptr = std::unique_ptr<SharedMemory>;

        static SharedMemory::Ptr create(const char *name, size_t size) noexcept;
        static SharedMemory::Ptr use(const char *name) noexcept;

        ~SharedMemory() noexcept;
        void *getBuffer() noexcept;

    private:
        SharedMemory() = default;

    private:
#ifdef WIN32
        HANDLE hMapFile = INVALID_HANDLE_VALUE;
#endif
        void *buffer = nullptr;
        bool isOwner{};
    };
}// namespace sese