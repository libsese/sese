#pragma once
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/Stream.h>
#include <sese/StringBuilder.h>

namespace sese::http {

    class API HttpUtil {
    public:
        static bool recvRequest(const Stream::Ptr &source, const RequestHeader::Ptr &request);
        static bool sendRequest(const Stream::Ptr &dest, const RequestHeader::Ptr &request);

        static bool recvResponse(const Stream::Ptr &source, const ResponseHeader::Ptr &response);
        static bool sendResponse(const Stream::Ptr &dest, const ResponseHeader::Ptr &response);

    private:
        static bool getLine(const Stream::Ptr &source, StringBuilder &builder);

        //todo 封装 recvHeader 和 sendHeader
    };
}