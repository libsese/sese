#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>

namespace sese::net::http {
    class Requestable : public io::InputStream, public io::OutputStream {
    public:
        ~Requestable() override = default;

        virtual bool init(const std::string &url, const std::string &proxy) = 0;

        virtual bool request() = 0;

        virtual int getLastError() = 0;

        virtual std::string getLastErrorString() = 0;

        int64_t read(void *buf, size_t len) override = 0;

        int64_t write(const void *buf, size_t len) override = 0;

        Request::Ptr &getRequest() {return req;}

        Response::Ptr &getResponse() {return resp;}

    protected:
        Request::Ptr req = nullptr;
        Response::Ptr resp = nullptr;
    };
}