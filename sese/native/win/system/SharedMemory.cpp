#include <sese/system/SharedMemory.h>

#include <windows.h>

using namespace sese::system;

SharedMemory::Ptr SharedMemory::create(const char *name, size_t size) noexcept {
    auto mapping = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            nullptr,
            PAGE_READWRITE,
            0,
            static_cast<DWORD>(size),
            name
    );

    if (nullptr == mapping) {
        return nullptr;
    }

    auto buffer = MapViewOfFile(
            mapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            size
    );

    if (nullptr == buffer) {
        return nullptr;
    }

    auto rt = MAKE_UNIQUE_PRIVATE(SharedMemory);
    rt->isOwner = true;
    rt->hMapFile = mapping;
    rt->buffer = buffer;
    return rt;
}

SharedMemory::Ptr SharedMemory::use(const char *name) noexcept {
    auto mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, name);
    if (nullptr == mapping) {
        return nullptr;
    }

    auto buffer = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (nullptr == buffer) {
        return nullptr;
    }

    auto rt = MAKE_UNIQUE_PRIVATE(SharedMemory);
    rt->isOwner = false;
    rt->hMapFile = mapping;
    rt->buffer = buffer;
    return rt;
}

SharedMemory::~SharedMemory() noexcept {
    UnmapViewOfFile(buffer);
    if (isOwner) {
        CloseHandle(hMapFile);
    }
}

void *SharedMemory::getBuffer() noexcept {
    return buffer;
}