#include "sese/util/Test.h"
#include "sese/record/LogHelper.h"

//#undef assert
//
//using sese::Test;
//using sese::TestInitiateTask;
//
//static std::atomic_size_t success = 0;
//static std::atomic_size_t times = 0;
//
//int32_t TestInitiateTask::init() noexcept {
//    return 0;
//}
//
//int32_t TestInitiateTask::destroy() noexcept {
//    if (ENABLE_TEST && times != 0) {// NOLINT
//        puts("==============TEST==============");
//        size_t i;
//        printf("Number of all assert: %zu\n", i = times);
//        printf("Number of success assert: %zu\n", i = success);
//        printf("Number of fail assert: %zu\n", times - success);
//        printf("Success rate of all assert: %.3f%%\n", ((double) success / (double) times) * 100);
//    }
//    return 0;
//}
//
//void Test::assert(record::LogHelper log, bool expr, int32_t exitCode) {
//    // if (!expr) {
//    //     times++;
//    //     log.error("Call assertion!\n%s",backtrace2String(5, "Backtrace ").c_str());
//    //     if (exitCode) exit(exitCode);
//    // } else {
//    //     success++;
//    //     times++;
//    // }
//    assert(log, "Assertion failed!\n%s", expr, exitCode);
//}
//
//void sese::Test::assert(record::LogHelper log, const char *firstLine, bool expr, int32_t exitCode) {
//    if (!expr) {
//        times++;
//        log.error(firstLine, sese::Test::backtrace2String(5, "Backtrace ").c_str());
//        if (exitCode) exit(exitCode);
//    } else {
//        success++;
//        times++;
//    }
//}