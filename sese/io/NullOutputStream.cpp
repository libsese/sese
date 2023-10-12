#include <sese/io/NullOutputStream.h>

int64_t sese::io::NullOutputStream::write([[maybe_unused]] const void *buffer, size_t length) {
    return static_cast<int64_t>(length);
}
