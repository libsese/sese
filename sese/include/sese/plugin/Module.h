#pragma once

#include "sese/plugin/BaseClass.h"
#include "sese/system/LibraryLoader.h"

namespace sese::plugin {

    class API Module {
    public:
        using Ptr = std::unique_ptr<Module>;

        static Module::Ptr open(const std::string &path) noexcept;

        const char *getName() noexcept;

        const char *getVersion() noexcept;

        const char *getDescription() noexcept;

        BaseClass::Ptr createClassWithId(const std::string &id) noexcept;

        template<typename type>
        std::shared_ptr<type> createClassWithIdAs(const std::string &id) noexcept {
            auto p = createClassWithId(id);
            if (p) {
                return std::reinterpret_pointer_cast<type>(p);
            } else {
                return nullptr;
            }
        }

    private:
        Module() = default;

        LibraryObject::Ptr object = nullptr;

        void *info = nullptr;
        void *factory = nullptr;
    };
}