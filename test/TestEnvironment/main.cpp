#include "record/LogHelper.h"
#include "system/Environment.h"

using sese::Environment;
using sese::LogHelper;

LogHelper helper("fENV"); // NOLINT

int main() {
    helper.info("Your operating system: %s",
                Environment::getOperateSystemType());

    helper.info("libsese version: %d.%d.%ld",
                Environment::getMajorVersion(),
                Environment::getMinorVersion(),
                Environment::getPatchVersion());

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