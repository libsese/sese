/// \file DNSSession.h
/// \author kaoru
/// \brief DNS 会话类
/// \version 0.1
/// \date 2023年8月13日

#pragma once

#include <sese/net/dns/Query.h>
#include <sese/net/dns/Answer.h>

#include <vector>

namespace sese::net::dns {

    /// DNS 会话类
    class API DNSSession {
    public:
        [[nodiscard]] auto &getQueries() { return queries; }
        [[nodiscard]] auto &getAnswers() { return answers; }

    protected:
        std::vector<Query> queries;
        std::vector<Answer> answers;
    };
}// namespace sese::net::dns