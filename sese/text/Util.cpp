#include <sese/text/Util.h>

#include <cstdarg>

int sese::text::snprintf(char *buf, size_t buf_size, const char *pattern, ...) {
    int result;
    va_list args;
    va_start(args, pattern);
#if defined(SESE_PLATFORM_WINDOWS)
    result = vsprintf_s(buf, buf_size, pattern, args);
#elif defined(SESE_PLATFORM_APPLE)
    result = vsnprintf(buf, buf_size, pattern, args);
#else
    result = std::vsnprintf(buf, buf_size, pattern, args);
#endif
    va_end(args);
    return result;
}
