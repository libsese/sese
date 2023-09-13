/// \file RpcService.h
/// \brief RPC 服务
/// \date 2023年8月5日
/// \author kaoru

#pragma once

#include <sese/security/SecuritySocket.h>
#include <sese/Config.h>
#include <sese/config/json/JsonTypes.h>
#include <sese/security/SSLContext.h>
#include <sese/service/TimerableService.h>
#include "sese/io/ByteBuilder.h"

#include <map>
#include <functional>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    /// RPC 服务
    class API RpcService final : public sese::event::EventLoop {
    public:
        using Func = std::function<void(json::ObjectData::Ptr &, json::ObjectData::Ptr &)>;

        explicit RpcService(const security::SSLContext::Ptr &context) noexcept;

        ~RpcService() noexcept override;

        void setFunction(const std::string &name, const Func &func) noexcept;

        void onAccept(int fd) override;

        void onRead(event::BaseEvent *event) override;

        void onWrite(event::BaseEvent *event) override;

        void onClose(event::BaseEvent *event) override;

    private:
        static int64_t read(int fd, void *buffer, size_t len, void *ssl) noexcept;

        static int64_t write(int fd, const void *buffer, size_t len, void *ssl) noexcept;

        bool onHandle(io::ByteBuilder *builder);

        security::SSLContext::Ptr context;

        std::map<int, io::ByteBuilder *> buffers;
        std::map<std::string, Func> funcs;
    };
}// namespace sese::service