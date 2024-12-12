// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file TcpTransporter.h
/// \brief TCP Transporter
/// \author kaoru
/// \version

#pragma once

#include <sese/service/TimerableService_V1.h>
#include <sese/security/SSLContext.h>
#include "sese/io/ByteBuilder.h"

#include <atomic>

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::service {

/// TCP Connection
struct TcpConnection {
    virtual ~TcpConnection() = default;

    /// \anchor tcp_connection_delay_close_by_async
    /// \brief
    /// Set this variable to indicate that this connection is being asynchronously processed.
    /// When an onClose event occurs, resource release operations will not be performed.
    /// The release operation will be delayed at least until the disconnection is detected
    /// during a write or read event (onWrite, onRead).
    /// After asynchronous operations are completed, this flag should be set back to false.
    std::atomic_bool isAsync = false;

    void *ssl = nullptr;
    event::BaseEvent *event = nullptr;
    service::v1::TimeoutEvent *timeoutEvent = nullptr;
    io::ByteBuilder buffer2read{8192};
    io::ByteBuilder buffer2write{8192};
};

/// TCP transporter configuration
struct TcpTransporterConfig {
    uint32_t keepalive = 30;
    security::SSLContext::Ptr servCtx = nullptr;

    virtual ~TcpTransporterConfig() = default;
    virtual TcpConnection *createConnection() = 0;
    virtual void freeConnection(TcpConnection *conn);
};

/// TCP transporter
class TcpTransporter : public v1::TimerableService {
public:
    explicit TcpTransporter(TcpTransporterConfig *transporter_config) noexcept;
    ~TcpTransporter() override;

protected:
    void onAccept(int fd) override;
    void onRead(event::BaseEvent *event) override;
    void onWrite(event::BaseEvent *event) override;
    void onClose(event::BaseEvent *event) override;
    void onTimeout(v1::TimeoutEvent *timeout_event) override;

protected:
    virtual void postRead(TcpConnection *conn);
    virtual void postWrite(TcpConnection *conn);
    virtual int onProcAlpnSelect(
            const uint8_t **out, uint8_t *out_length,
            const uint8_t *in, uint32_t in_length
    ) = 0;
    virtual void onProcAlpnGet(
            TcpConnection *conn,
            const uint8_t *in, uint32_t in_length
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
            const uint8_t **out, uint8_t *out_length,
            const uint8_t *in, uint32_t in_length,
            TcpTransporter *transporter
    );
};

} // namespace sese::service