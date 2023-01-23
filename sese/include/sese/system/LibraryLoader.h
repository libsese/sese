/**
 * @file LibraryLoader.h
 * @author kaoru
 * @brief 外部库加载器
 * @date 2022年4月22日
 */
#pragma once
#include "sese/Config.h"
#include "sese/util/NotInstantiable.h"

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese {

    /**
     * @brief 库对象
     */
    class API LibraryObject {
    public:
        using Ptr = std::shared_ptr<LibraryObject>;
#ifdef _WIN32
        using Module = HMODULE;
#else
        using Module = void *;
#endif

        explicit LibraryObject(Module module) noexcept;
        ~LibraryObject() noexcept;
        /**
         * @brief 根据名称返回库中函数指针
         * @param name 函数名称
         * @return 函数指针，找不到为 nullptr
         */
        [[nodiscard]] const void *findFunctionByName(const std::string &name) const;

    private:
        Module module;
    };

    /**
     * @brief 库对象加载器
     */
    class API LibraryLoader : public NotInstantiable {
    public:
        /**
         * @brief 加载一个外部库
         * @param name 库名称
         * @return 库对象，打开失败返回 nullptr
         */
        static LibraryObject::Ptr open(const std::string &name) noexcept;
    };

}// namespace sese