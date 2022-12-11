#include <sese/Initializer.h>
#include <sese/record/LogHelper.h>

using sese::Initializer;
using sese::record::LogHelper;

int main() {
    // 程序使用静态链接时，需要手动进行初始化；
    // 使用动态链接时，手动进行初始化也无妨。
    Initializer::getInitializer();

    LogHelper helper("example");// NOLINT
    helper.info("This message sent by sese::LogHelper!");

    // do something

    return 0;
}