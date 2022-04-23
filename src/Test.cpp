#include "sese/Test.h"
#include "sese/record/LogHelper.h"

using sese::LogHelper;
using sese::Test;

static std::atomic_size_t success = 0;
static std::atomic_size_t times = 0;

namespace sese {
    static struct TestInitStruct {
        ~TestInitStruct() {
            if (ENABLE_TEST && times != 0) {// NOLINT
                puts("==============TEST==============");
                size_t i;
                printf("Number of all assert: %zu\n", i = times);
                printf("Number of success assert: %zu\n", i = success);
                printf("Number of fail assert: %zu\n", times - success);
                printf("Success rate of all assert: %.3f%%", ((double) success / (double) times) * 100);
            }
        }
    } testInitStruct;// NOLINT
}// namespace sese

void Test::assert(LogHelper log, bool expr, int32_t exitCode) {
    if (!expr) {
        times++;
        log.error("Call assertion!\n%s",backtrace2String(5, "Backtrace ").c_str());
        if (exitCode) exit(exitCode);
    } else {
        success++;
        times++;
    }
}