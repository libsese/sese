#include "sese/system/LibraryLoader.h"
#include "sese/record/LogHelper.h"

#include "gtest/gtest.h"

using Func = double(double);

TEST(TestLibraryLoader, _0) {
#ifdef _WIN32
    auto lib_name = "NTDLL.DLL";
#elif __linux__
    auto *lib_name = "libstdc++.so.6";
#elif __APPLE__
    auto *lib_name = "libstdc++.6.dylib";
#endif
    sese::record::LogHelper::i("loading lib \"%s\"", lib_name);
    const auto OBJECT = sese::system::LibraryObject::create(lib_name);
    ASSERT_NE(OBJECT, nullptr);
    const auto SIN = (Func *) (OBJECT->findFunctionByName("sin"));
    ASSERT_NE(SIN, nullptr);

    const auto A = SIN(1.0f);
    const auto B = SIN(0.0f);
    sese::record::LogHelper::i("sin(1.0f) = %f", A);
    sese::record::LogHelper::i("sin(0.0f) = %f", B);
}