#pragma once
#include "Config.h"
#include "util/NotInstantiable.h"

namespace sese {

    class API Environment : NotInstantiable {
    public:
        static const char *getRepoHash() noexcept;

        static const char *getRepoBranch() noexcept;

        static int getMajorVersion() noexcept;

        static int getMinorVersion() noexcept;

        static const char *getBuildDate() noexcept;

        static const char *getBuildTime() noexcept;

        static const char *getBuildTimestamp() noexcept;

        static const char *getOperateSystemType() noexcept;
    };
}// namespace sese