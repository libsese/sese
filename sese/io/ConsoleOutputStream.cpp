#include "sese/io/ConsoleOutputStream.h"

int64_t sese::io::ConsoleOutputStream::write(const void *buffer, size_t length) {
    return static_cast<int64_t>(fwrite(buffer, 1, length, stdout));
}