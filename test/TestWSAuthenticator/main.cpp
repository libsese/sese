#include "sese/net/ws/WebSocketAuthenticator.h"
#include "sese/record/LogHelper.h"
#include "sese/util/Test.h"

const char *key = "fWx2f7uUjWwEuKkSyZAFNA==";
const char *result = "44m0OOuQtpbFH+BhSl9iSIQ/s4E=";

int main() {
    sese::record::LogHelper log("WSAuth");

    auto rt0 = sese::net::ws::WebSocketAuthenticator::verify(key, result);
    assert(log, rt0, 0);

    auto pair = sese::net::ws::WebSocketAuthenticator::generateKeyPair();
    log.debug("key: %s", pair.first.get());
    log.debug("res: %s", pair.second.get());
    auto rt1 = sese::net::ws::WebSocketAuthenticator::verify(pair.first.get(), pair.second.get());
    assert(log, rt1, 0);
    if (rt1) {
        log.debug("The verification is successful");
    }

    return 0;
}