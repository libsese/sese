#include "sese/record/LogHelper.h"
#include "sese/system/Environment.h"

using sese::Environment;
using sese::LogHelper;

LogHelper helper("fENV"); // NOLINT

int main() {
    helper.info("Your operating system: %s",
                Environment::getOperateSystemType());

    helper.info("libsese version: %d.%d.%s",
                Environment::getMajorVersion(),
                Environment::getMinorVersion(),
                Environment::getRepoHash());

    helper.info("libsese repo on the %s branch: %s",
                Environment::getRepoBranch(),
                Environment::getRepoHash());

    helper.info("libsese build on %s %s - %s",
                Environment::getBuildDate(),
                Environment::getBuildTime(),
                Environment::getBuildDateTime());

    helper.info("system endian: %s",
                Environment::isLittleEndian() ? "little endian" : "big endian");

    return 0;
}