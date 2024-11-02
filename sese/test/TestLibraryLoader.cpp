#define SESE_C_LIKE_FORMAT

#include "sese/system/LibraryLoader.h"
#include "sese/Log.h"

#include "gtest/gtest.h"

using Func = double(double);

TEST(TestLibraryLoader, Sin) {
#ifdef _WIN32
    auto lib_name = "NTDLL.DLL";
#elif __linux__
    auto *lib_name = "libstdc++.so.6";
#elif __APPLE__
    auto *lib_name = "libstdc++.6.dylib";
#endif
    SESE_INFO("loading lib \"%s\"", lib_name);
    const auto OBJECT = sese::system::LibraryObject::create(lib_name);
    ASSERT_NE(OBJECT, nullptr);
    const auto SIN = (Func *) (OBJECT->findFunctionByName("sin"));
    ASSERT_NE(SIN, nullptr);

    const auto A = SIN(1.0f);
    const auto B = SIN(0.0f);
    SESE_INFO("sin(1.0f) = %f", A);
    SESE_INFO("sin(0.0f) = %f", B);
}

TEST(TestLibraryLoader, Result) {
    auto result = sese::sys::LibraryObject::createEx("undef.dll");
    if (result) {
        SESE_INFO("%s", result.err().message().c_str());
        return;
    }
    auto lib = result.get();
}