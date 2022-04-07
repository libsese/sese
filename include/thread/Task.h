#pragma once
#include "Config.h"
#include <functional>
#include <memory>

namespace sese {
    class API Task{
    public:
        using Ptr = std::shared_ptr<Task>;

        [[nodiscard]] virtual std::function<void()> getFunction() const noexcept = 0;
    };
}