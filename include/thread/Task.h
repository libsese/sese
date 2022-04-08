#pragma once
#include "Config.h"
#include <functional>
#include <memory>

namespace sese {
    class API Task {
    public:
        using Ptr = std::shared_ptr<Task>;

        virtual void content() noexcept = 0;

        [[nodiscard]] virtual std::function<void()> getFunction() noexcept {
            return {[this] { content(); }};
        }
    };
}// namespace sese