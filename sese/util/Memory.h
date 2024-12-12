// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file Memory.h
 * @author kaoru
 * @brief Wrapper for memory-related APIs
 * @date June 5, 2022
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
 * Instantiate SharedType
 * @tparam RETURN_TYPE Template parameter for SharedType
 * @tparam ARGS Constructor parameter types for the template parameter
 * @param args Constructor parameters for the template parameter
 * @return SharedType<ReturnType>
 */
template<typename RETURN_TYPE, typename... ARGS>
inline SharedType<RETURN_TYPE> makeShared(ARGS &&...args) {
    return std::make_shared<RETURN_TYPE>(args...);
}

/**
 * Instantiate UniqueType
 * @tparam RETURN_TYPE Template parameter for UniqueType
 * @tparam ARGS Constructor parameter types for the template parameter
 * @param args Constructor parameters for the template parameter
 * @return Unique
 */
template<typename RETURN_TYPE, typename... ARGS>
inline SharedType<RETURN_TYPE> makeUnique(ARGS &&...args) {
    return std::make_unique<RETURN_TYPE>(args...);
}

#define MAKE_SHARED_PRIVATE(RETURN_TYPE, ...) sese::SharedType<RETURN_TYPE>(new RETURN_TYPE(__VA_ARGS__))

#define MAKE_UNIQUE_PRIVATE(RETURN_TYPE, ...) sese::UniqueType<RETURN_TYPE>(new RETURN_TYPE(__VA_ARGS__))

/**
 * Using std::make_shared with std::initializer_list
 * @tparam RETURN_TYPE Template type expected to return std::shared_ptr
 * @tparam INIT_TYPE Template parameter for std::initializer_list
 * @param list Initialization list
 * @return std::shared_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename INIT_TYPE>
SharedType<RETURN_TYPE> makeSharedFromList(std::initializer_list<INIT_TYPE> list) {
    return std::make_shared<RETURN_TYPE>(std::move(list));
}

/**
 * Using std::make_unique with std::initializer_list
 * @tparam RETURN_TYPE Template parameter expected to return std::unique_ptr
 * @tparam INIT_TYPE Template parameter for std::initializer_list
 * @param list Initialization list
 * @return std::unique_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename INIT_TYPE>
UniqueType<RETURN_TYPE> makeUniqueFromList(std::initializer_list<INIT_TYPE> list) {
    return std::make_unique<RETURN_TYPE>(std::move(list));
}

/**
 * Type casting for std::shared_ptr
 * @tparam RETURN_TYPE Template parameter expected to return std::shared_ptr
 * @tparam RAW_TYPE Template parameter of the original std::shared_ptr
 * @param raw_type std::shared_ptr to convert
 * @return std::shared_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename RAW_TYPE>
inline SharedType<RETURN_TYPE> dynamicPointerCast(SharedType<RAW_TYPE> raw_type) {
    return std::dynamic_pointer_cast<RETURN_TYPE>(raw_type);
}

/**
 * Type casting for std::unique_ptr
 * @tparam RETURN_TYPE Template parameter expected to return std::unique_ptr
 * @tparam RAW_TYPE Template parameter of the original std::unique_ptr
 * @param raw_type std::unique_ptr to convert
 * @return std::unique_ptr<ReturnType>
 */
template<typename RETURN_TYPE, typename RAW_TYPE>
UniqueType<RETURN_TYPE> dynamicPointerCast(UniqueType<RAW_TYPE> &raw_type) {
    RAW_TYPE *pointer = raw_type.release();
    RETURN_TYPE *newPointer = dynamic_cast<RETURN_TYPE *>(pointer); // NOLINT
    return std::unique_ptr<RETURN_TYPE>(newPointer);
}
} // namespace sese