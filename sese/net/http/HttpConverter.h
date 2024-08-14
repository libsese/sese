#pragma once

#include <sese/util/NotInstantiable.h>

#include "Request.h"

namespace sese::net::http {

    class HttpConverter final : public NotInstantiable {
    public:
        static void convertFromHttp2(Request *request);
    };

}
