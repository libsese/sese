#include "sese/system/LibraryLoader.h"
#include "sese/record/LogHelper.h"

#include "gtest/gtest.h"

using Func = double(double);

TEST(TestLibraryLoader, _0) {
#ifdef _WIN32
    const char *libName = "NTDLL.DLL";
#elif __linux__
    const char *libName = "libstdc++.so.6";
#elif __APPLE__
    const char *libName = "libstdc++.6.dylib";
#endif
    sese::record::LogHelper::i("loading lib \"%s\"", libName);
    auto object = sese::system::LibraryObject::create(libName);
    ASSERT_NE(object, nullptr);
    auto sin = (Func *) object->findFunctionByName("sin");
    ASSERT_NE(sin, nullptr);

    auto a = sin(1.0f);
    auto b = sin(0.0f);
    sese::record::LogHelper::i("sin(1.0f) = %f", a);
    sese::record::LogHelper::i("sin(0.0f) = %f", b);
}