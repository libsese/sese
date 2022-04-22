#include "system/LibraryLoader.h"
#include "record/LogHelper.h"
#include "Test.h"

using sese::Test;
using sese::LibraryLoader;
using sese::LibraryObject;
using sese::LogHelper;
using Func = double(double);

LogHelper helper("fLIB_LOADER"); // NOLINT

int main() {
#ifdef _WIN32
    const char *libName = "NTDLL.DLL";
#elif __linux__
    const char *libName = "libstdc++.so.6";
#elif __APPLE__
    const char *libName = "libstdc++.6.dylib";
#endif
    helper.info("loading lib \"%s\"", libName);
    auto object = LibraryLoader::open(libName);
    Test::assert(helper, object != nullptr);
    auto sin = (Func *)object->findFunctionByName("sin");
    Test::assert(helper, sin != nullptr);

    auto a = sin(1.0f);
    auto b = sin(0.0f);
    helper.info("sin(1.0f) = %f", a);
    helper.info("sin(0.0f) = %f", b);

    return 0;
}