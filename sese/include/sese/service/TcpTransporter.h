/// \file TcpTransporter.h
/// \brief TCP 传输器
/// \author kaoru
/// \version 0.1
/// \date 日期

#pragma once

#include <sese/service/TimerableService.h>
#include <sese/security/SSLContext.h>
#include "sese/io/ByteBuilder.h"

#include <atomic>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

    /// TCP 连接
    struct TcpConnection {
        virtual ~TcpConnection() = default;

        /// \anchor tcp_connection_delay_close_by_async
        /// \brief
        /// 设置此变量表明此连接正在进行异步处理，
        /// 遇到对端关闭事件（onClose）时，不进行资源释放处理，
        /// 释放操作至少会延迟至写入事件或读取事件（onWrite、onRead）时检测到连接断开处理，
        /// 异步操作结束时，应当重新将此标志设置为 false
        std::atomic_bool isAsync = false;

        void *ssl = nullptr;
        event::BaseEvent *event = nullptr;
        service::TimeoutEvent *timeoutEvent = nullptr;
        io::ByteBuilder buffer2read{8192};
        io::ByteBuilder buffer2write{8192};
    };

    /// TCP 传输器配置
    struct API TcpTransporterConfig {
        uint32_t keepalive = 30;
        security::SSLContext::Ptr servCtx = nullptr;

        virtual TcpConnection *createConnection() = 0;
        virtual void freeConnection(TcpConnection *conn);
    };

    /// TCP 传输器
    class API TcpTransporter : public TimerableService {
    public:
        explicit TcpTransporter(TcpTransporterConfig *transporterConfig) noexcept;
        ~TcpTransporter() override;

    protected:
        void onAccept(int fd) override;
        void onRead(event::BaseEvent *event) override;
        void onWrite(event::BaseEvent *event) override;
        void onClose(event::BaseEvent *event) override;
        void onTimeout(TimeoutEvent *timeoutEvent) override;

    protected:
        virtual void postRead(TcpConnection *conn);
        virtual void postWrite(TcpConnection *conn);
        virtual int onProcAlpnSelect(
                const uint8_t **out, uint8_t *outLength,
                const uint8_t *in, uint32_t inLength
        ) = 0;
        virtual void onProcAlpnGet(
                TcpConnection *conn,
                const uint8_t *in, uint32_t inLength
        ) = 0;
        virtual void onProcHandle(TcpConnection *conn) = 0;
        virtual void onProcClose(TcpConnection *conn) = 0;

    protected:
        event::BaseEvent *createEventEx(int fd, unsigned int events, TcpConnection *conn) noexcept;
        void freeEventEx(event::BaseEvent *event) noexcept;
        static int64_t read(int fd, void *buffer, size_t len, void *ssl) noexcept;
        static int64_t write(int fd, const void *buffer, size_t len, void *ssl) noexcept;

        TcpTransporterConfig *config = nullptr;
        std::map<int, event::BaseEvent *> eventMap;

    private:
        static int alpnCallbackFunction(
                void *ssl,
                const uint8_t **out, uint8_t *outLength,
                const uint8_t *in, uint32_t inLength,
                TcpTransporter *transporter
        );
    };

}// namespace sese::service