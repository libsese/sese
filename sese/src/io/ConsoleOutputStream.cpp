#include "sese/io/ConsoleOutputStream.h"

int64_t sese::io::ConsoleOutputStream::write(const void *buffer, size_t length) {
    return fwrite(buffer, length, 1, stdout);
}