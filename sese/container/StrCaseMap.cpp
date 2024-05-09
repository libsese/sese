#include <sese/container/StrCaseMap.h>
#include <sese/Util.h>

#include <algorithm>

#include <sese/record/Marco.h>

bool sese::StrCaseMapComparer::operator()(const std::string &lv, const std::string &rv) const {
    return sese::StrCmpI()(lv, rv) < 0;
}

size_t sese::StrCaseUnorderedMapComparer::operator()(const std::string &key) const {
#if defined(SESE_PLATFORM_WINDOWS) && !defined(__MINGW32__)
#define XX std::tolower
#else
#define XX ::tolower
#endif
    auto temp = key;
    std::transform(temp.begin(), temp.end(), temp.begin(), XX);
    return std::hash<std::string>{}(temp);
#undef XX
}

bool sese::StrCaseUnorderedMapComparer::operator()(const std::string &lv, const std::string &rv) const {
    return sese::StrCmpI()(lv, rv) == 0;
}