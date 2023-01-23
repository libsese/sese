#include <sese/system/SharedMemory.h>
#include "sese/util/ByteBuilder.h"
#include <sese/convert/MD5Util.h>

using namespace sese;

key_t SharedMemory::name2key(const char *name) noexcept {
    auto builder = std::make_shared<ByteBuilder>();
    builder->write(name, strlen(name));
    auto str = MD5Util::encode(builder);

    char num[]{
            str[1],
            str[3],
            str[2],
            str[5],
            str[7],
            str[11],
            str[13],
            0};
    std::size_t size;
    return std::stoi(num, &size, 16);
}

SharedMemory::~SharedMemory() noexcept {
    shmdt(buffer);
    if (isOwner) {
        shmctl(id, IPC_RMID, nullptr);
    }
}

SharedMemory::Ptr SharedMemory::create(const char *name, size_t size) noexcept {
    int32_t key = name2key(name);
    int32_t id = shmget((key_t) key, size, 0666 | IPC_CREAT);
    if (-1 == id) {
        return nullptr;
    }

    auto buffer = shmat(id, nullptr, 0);
    if (buffer == (void *) -1) {
        return nullptr;
    }

    auto rt = new SharedMemory;
    rt->isOwner = true;
    rt->id = id;
    rt->buffer = buffer;
    return std::unique_ptr<SharedMemory>(rt);
}

SharedMemory::Ptr SharedMemory::use(const char *name) noexcept {
    int32_t key = name2key(name);
    int32_t id = shmget(key, 0, 0666);
    if (-1 == id) {
        return nullptr;
    }

    auto buffer = shmat(id, nullptr, 0);
    if (buffer == (void *) -1) {
        return nullptr;
    }

    auto rt = new SharedMemory;
    rt->isOwner = false;
    rt->id = id;
    rt->buffer = buffer;
    return std::unique_ptr<SharedMemory>(rt);
}

void *SharedMemory::getBuffer() noexcept {
    return buffer;
}