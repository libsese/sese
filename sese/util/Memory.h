/**
 * @file Memory.h
 * @author kaoru
 * @brief 内存相关 API 的包装
 * @date 2022年6月5日
 */
#pragma once
#include <memory>

#ifdef _WIN32
#pragma warning(disable : 4819)
#endif

namespace sese {

template<typename T>
using SharedType = std::shared_ptr<T>;

template<typename T>
using UniqueType = std::unique_ptr<T>;

template<typename T>
using WeakType = std::weak_ptr<T>;

/**
 * 实例化 SharedType
 * @tparam ReturnType SharedType 模板参数
 * @tparam Args 模板参数的构造函数参数类型
 * @param args 模板参数的构造函数参数
 * @return SharedType<ReturnType>
 */
template<typename RETURN_TYPE, typename... ARGS>
inline SharedType<RETURN_TYPE> makeShared(ARGS &&...args) {
    return std::make_shared<RETURN_TYPE>(args...);
}

/**
 * 实例化 UniqueType
 * @tparam ReturnType UniqueType 模板参数
 * @tparam Args 模板参数的构造函数参数类型
 * @param args 模板参数的构造函数参数
 * @return UniqueType<ReturnType>
 */
template<typename RETURN_TYPE, typename... ARGS>
inline SharedType<RETURN_TYPE> makeUnique(ARGS &&...args) {
    return std::make_unique<RETURN_TYPE>(args...);
}

/**
 * std::make_shared 搭配 std::initializer_list 使用
 * @tparam ReturnType 预计返回 std::shared_ptr 的模板类型
 * @tparam InitType std::initializer_list 模板参数
 * @param list 初始化列表
 * @return std::shared_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename INIT_TYPE>
SharedType<RETURN_TYPE> makeSharedFromList(std::initializer_list<INIT_TYPE> list) {
    return std::make_shared<RETURN_TYPE>(std::move(list));
}

/**
 * std::make_unique 搭配 std::initializer_list 使用
 * @tparam ReturnType 预计返回 std::unique_ptr 的模板参数
 * @tparam InitType std::initializer_list 的模板参数
 * @param list 初始化列表
 * @return std::unique_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename INIT_TYPE>
UniqueType<RETURN_TYPE> makeUniqueFromList(std::initializer_list<INIT_TYPE> list) {
    return std::make_unique<RETURN_TYPE>(std::move(list));
}

/**
 * std::shared_ptr 的类型转换
 * @tparam ReturnType 预计返回 std::shared_ptr 的模板参数
 * @tparam RawType 原始的 std::shared_ptr 的模板参数
 * @param rawType 需要转换的 std::shared_ptr
 * @return std::shared_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename RAW_TYPE>
inline SharedType<RETURN_TYPE> dynamicPointerCast(SharedType<RAW_TYPE> raw_type) {
    return std::dynamic_pointer_cast<RETURN_TYPE>(raw_type);
}

/**
 * std::unique_ptr 的类型转换
 * @tparam ReturnType 预计返回 std::unique_ptr 的模板参数
 * @tparam RawType 原始的 std::unique_ptr 的模板参数
 * @param rawType 需要转换的 std::unique_ptr
 * @return std::unique_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename RAW_TYPE>
UniqueType<RETURN_TYPE> dynamicPointerCast(UniqueType<RAW_TYPE> &raw_type) {
    RAW_TYPE *pointer = raw_type.release();
    RETURN_TYPE *newPointer = dynamic_cast<RETURN_TYPE *>(pointer); // NOLINT
    return std::unique_ptr<RETURN_TYPE>(newPointer);
}
} // namespace sese