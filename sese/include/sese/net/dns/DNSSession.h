#pragma once

#include <sese/net/dns/Query.h>
#include <sese/net/dns/Answer.h>

namespace sese::net::dns {

    class API DNSSession {
    public:
        [[nodiscard]] auto &getQueries() { return queries; }
        [[nodiscard]] auto &getAnswers() { return answers; }

    protected:
        std::vector<Query> queries;
        std::vector<Answer> answers;
    };
}// namespace sese::net::dns