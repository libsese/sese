/**
 * @file LibraryLoader.h
 * @author kaoru
 * @brief 外部库加载器
 * @date 2022年4月22日
 */

#pragma once

#include "sese/Config.h"
#include "sese/system/Path.h"

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese::system {

/**
 * @brief 库对象
 */
class  LibraryObject {
public:
    using Ptr = std::shared_ptr<LibraryObject>;
#ifdef _WIN32
    using Module = HMODULE;
#else
    using Module = void *;
#endif
    /**
     * @brief 加载一个外部库
     * @param name 库名称
     * @return 库对象，打开失败返回 nullptr
     */
    static LibraryObject::Ptr create(const std::string &name) noexcept;

    /**
     * @brief 加载一个外部库
     * @param path 库路径
     * @return 库对象，打开失败返回 nullptr
     */
    static LibraryObject::Ptr createWithPath(const system::Path &path) noexcept;

    ~LibraryObject() noexcept;
    /**
     * @brief 根据名称返回库中函数指针
     * @param name 函数名称
     * @return 函数指针，找不到为 nullptr
     */
    [[nodiscard]] const void *findFunctionByName(const std::string &name) const;

private:
    explicit LibraryObject(Module module) noexcept;

    Module module;
};

} // namespace sese::system