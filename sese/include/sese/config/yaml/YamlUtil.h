/// \file YamlUtil.h
/// \author kaoru
/// \date 2023年6月30日
/// \brief yaml 工具类
/// \version 0.1

#pragma once

#include "sese/util/InputStream.h"
#include "sese/util/OutputStream.h"
#include "sese/util/NotInstantiable.h"
#include "sese/config/yaml/YamlTypes.h"

#include <tuple>
#include <vector>
#include <queue>

namespace sese::yaml {

    class API YamlUtil final : public NotInstantiable {
    public:
        using Line = std::tuple<int, std::string>;
        using Tokens = std::vector<std::string>;
        using TokensQueue = std::queue<std::tuple<int, Tokens>>;

        /// 从流中反序列化 yaml 对象
        /// \param input 输入流
        /// \param level 解析深度
        /// \retval nullptr 反序列化错误
        /// \return yaml 对象
        static Data::Ptr deserialize(InputStream *input, size_t level) noexcept;

        /// 向流中序列化 yaml 对象
        /// \param data yaml 对象
        /// \param output 输出流
        static void serialize(const Data::Ptr &data, OutputStream *output) noexcept;

#ifdef SESE_BUILD_TEST
    public:
#else
    private:
#endif
        static int getSpaceCount(const std::string &line) noexcept;

        static Line getLine(InputStream *input) noexcept;

        static Tokens tokenizer(const std::string &line) noexcept;

        static ObjectData::Ptr createObject(TokensQueue &tokensQueue, size_t level) noexcept;

        static ArrayData::Ptr createArray(TokensQueue &tokensQueue, size_t level) noexcept;

        static void writeSpace(size_t count, OutputStream *output) noexcept;

        static void serializeObject(ObjectData *objectData, OutputStream *output, size_t level) noexcept;

        static void serializeArray(ArrayData *arrayData, OutputStream *output, size_t level) noexcept;
    };
}// namespace sese::yaml