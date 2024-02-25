#pragma once

#include "sese/security/SSLContext.h"

namespace sese::service {

struct HttpServerConfig {
    security::SSLContext ssl_context;
};

}