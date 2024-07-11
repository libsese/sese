#include <sese/io/RandomInputStream.h>
#include <sese/util/RandomUtil.h>

#include <memory.h>

int64_t sese::io::RandomInputStream::read(void *buffer, size_t length) {
    size_t count = 0;
    while (count != length) {
        auto r = RandomUtil::next<uint64_t>();
        auto need = std::min<size_t>(8, length - count);
        memcpy((uint8_t *) buffer + count, &r, need);
        count += need;
    }
    return static_cast<int64_t>(length);
}
