#pragma once

#include "sese/util/InputStream.h"
#include "sese/util/OutputStream.h"
#include "sese/util/NotInstantiable.h"
#include "sese/config/yaml/YamlTypes.h"

#include <tuple>
#include <vector>

namespace sese::yaml {

    class YamlUtil final : public NotInstantiable {
    public:
        // static Data::Ptr deserialize(InputStream *input, size_t level) noexcept;

        // static void serialize(const Data::Ptr &data, OutputStream *output) noexcept;

        static int getSpaceCount(const std::string &line) noexcept;

        static std::tuple<int, std::string> getLine(InputStream *input) noexcept;

        static std::vector<std::string> tokenizer(const std::string &line) noexcept;
    };
}// namespace sese::yaml