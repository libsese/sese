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

/**
 * @file IOCPServer_V1.h
 * @brief IOCP server based on sese-event
 * @author kaoru
 * @version 0.1
 * @date September 25, 2023
 */

#pragma once

#include <sese/security/SSLContext.h>
#include <sese/service/TimerableService_V2.h>
#include <sese/service/UserBalanceLoader.h>
#include <sese/io/ByteBuilder.h>

#include <set>

namespace sese::iocp::v1 {

class Context;
class IOCPServer;
class IOCPService;

/// IOCP server based on sese-event
class IOCPServer {
public:
    virtual ~IOCPServer() = default;
    using DeleteContextCallback = std::function<void(Context *data)>;

    IOCPServer();

    /**
     * Initialize and start the server
     * @return Initialization result
     */
    bool init();
    /**
     * Terminate worker threads, release system resources, and shut down the server
     */
    void shutdown();
    /**
     * Post a read event
     * @param ctx Operation context
     */
    static void postRead(Context *ctx);
    /**
     * Post a write event
     * @param ctx Operation context
     */
    static void postWrite(Context *ctx);
    /**
     * Post a close event
     * @param ctx Operation context
     */
    static void postClose(Context *ctx);
    /**
     * Post a connection event
     * @param to Connection address
     * @param cli_ctx SSL client context
     * @param data Additional data
     */
    void postConnect(const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cli_ctx, void *data = nullptr);
    /**
     * Set a timeout event
     * @param ctx Operation context
     * @param seconds Timeout duration
     */
    static void setTimeout(Context *ctx, int64_t seconds);
    /**
     * Cancel the timeout event
     * @param ctx Operation context
     */
    static void cancelTimeout(Context *ctx);
    /**
     * Default context release callback function
     */
    static void onDeleteContext(Context *) {}
    /**
     * Handshake completed callback function
     * @param ctx Operation context
     */
    virtual void onAcceptCompleted(Context *ctx) {}
    /**
     * Callback function triggered by a read event
     * @param ctx Operation context
     */
    virtual void onPreRead(Context *ctx) {}
    /**
     * Callback function for completion of a read event
     * @param ctx Operation context
     */
    virtual void onReadCompleted(Context *ctx) {}
    /**
     * Write event completed callback function
     * @param ctx Operation context
     */
    virtual void onWriteCompleted(Context *ctx) {}
    /**
     * Timeout event callback function
     * @param ctx Operation context
     */
    virtual void onTimeout(Context *ctx) {}
    /**
     * Connection pre-event callback function
     * @param ctx Operation context
     */
    virtual void onPreConnect(Context *ctx) {}
    /**
     * Connection event callback function
     * @param ctx Operation context
     */
    virtual void onConnected(Context *ctx) {}
    /**
     * ALPN protocol negotiation completion callback function
     * @param ctx Context
     * @param in Negotiation content
     * @param in_length Length of negotiation content
     */
    virtual void onAlpnGet(Context *ctx, const uint8_t *in, uint32_t in_length) {}
    /**
     * ALPN negotiation callback function
     * @param out Expected content from the other side
     * @param out_length Length of expected content from the other side
     * @param in Response content
     * @param in_length Length of response content
     * @return ALPN status code
     */
    int onAlpnSelect(
            const uint8_t **out, uint8_t *out_length,
            const uint8_t *in, uint32_t in_length
    );

public:
    /**
     * Set the IP address to which the current service is bound. Setting this option will make the server automatically listen to the corresponding port of the address
     * @param addr Target IP
     */
    void setAddress(const net::IPAddress::Ptr &addr) { balanceLoader.setAddress(addr); }
    /**
     * Set the desired number of threads for the service
     * @param threads Number of threads
     */
    void setThreads(size_t threads) { balanceLoader.setThreads(threads); }
    /**
     * Set the SSL context used for server listening
     * @param ctx SSL context
     */
    void setServCtx(const security::SSLContext::Ptr &ctx) { IOCPServer::sslCtx = ctx; }
    /**
     * Set the ALPN negotiation content for the server
     * @param protos Protocol negotiation content
     */
    void setServProtos(const std::string &protos) { IOCPServer::servProtos = protos; }
    /**
     * Set the ALPN negotiation content for the client
     * @param protos Protocol negotiation content
     */
    void setClientProtos(const std::string &protos) { IOCPServer::clientProtos = protos; }
    /**
     * Set the server operation context destruction callback function
     * @param callback Callback function
     */
    void setDeleteContextCallback(const DeleteContextCallback &callback) { IOCPServer::deleteContextCallback = callback; }
    /**
     * Get the current SSL context for server listening
     * @return SSL context
     */
    [[nodiscard]] const security::SSLContext::Ptr &getServCtx() const { return IOCPServer::sslCtx; }
    /**
     * Get the current operation context destruction callback function for the service
     * @return Callback function
     */
    [[nodiscard]] const DeleteContextCallback &getDeleteContextCallback() const { return IOCPServer::deleteContextCallback; };

    /**
     * Get the status of the active release mode
     * @return Status of the active release mode
     */
    [[nodiscard]] bool isActiveReleaseMode() const { return activeReleaseMode; }

public:
    /**
     * Set the connection access timeout duration. This function is only for IOCP implementations based on sese-event
     * @param seconds Connection access timeout duration
     */
    [[maybe_unused]] void setAcceptTimeout(uint32_t seconds) { balanceLoader.setAcceptTimeout(seconds); }
    /**
     * Set the connection dispatch timeout duration. This function is only for IOCP implementations based on sese-event
     * @param seconds Connection dispatch timeout duration
     */
    [[maybe_unused]] void setDispatchTimeout(uint32_t seconds) { balanceLoader.setDispatchTimeout(seconds); }

protected:
    void preConnectCallback(int fd, sese::event::EventLoop *event_loop, Context *ctx);

    /**
     * Set active release mode
     * @param enable Enable or disable
     */
    void setActiveReleaseMode(bool enable) { activeReleaseMode = enable; }

    DeleteContextCallback deleteContextCallback = onDeleteContext;
    security::SSLContext::Ptr sslCtx{};
    std::string servProtos{};
    std::string clientProtos{};
    service::UserBalanceLoader balanceLoader;

private:
    bool activeReleaseMode = true;
};

/// Completion port operation context based on sese-event
class Context final : public io::InputStream, public io::OutputStream, public io::PeekableStream {
    friend class IOCPServer;
    friend class IOCPService;
    IOCPServer *self{};
    IOCPService *client{};
    socket_t fd{0};
    void *ssl{};
    bool isConn{false};
    event::BaseEvent *event{};
    service::v2::TimeoutEvent *timeoutEvent{};
    io::ByteBuilder send{8192, 8192};
    io::ByteBuilder recv{8192, 8192};
    void *data{};

public:
    /**
     * Read content from the current connection
     * @param buffer Buffer
     * @param length Size of buffer
     * @return Actual read size
     */
    int64_t read(void *buffer, size_t length) override;
    /**
     * Write content to the current connection
     * @param buffer Buffer
     * @param length Size of buffer
     * @return Actual written size
     */
    int64_t write(const void *buffer, size_t length) override;
    /**
     * Read content from the current connection without advancing
     * @param buffer Buffer
     * @param length Size of buffer
     * @return Actual read size
     */
    int64_t peek(void *buffer, size_t length) override;
    /**
     * Advance in the current connection without reading content
     * @param length Advance size
     * @return Actual advance size
     */
    int64_t trunc(size_t length) override;
    /**
     * Get the file descriptor of the current context connection
     * @return File descriptor
     */
    [[nodiscard]] int32_t getFd() const { return static_cast<int32_t>(Context::fd); }
    /**
     * Get additional data of the current context
     * @return Additional data
     */
    [[nodiscard]] void *getData() const { return Context::data; }
    /**
     * Set additional data for the current context
     * @param p_data Additional data
     */
    [[maybe_unused]] void setData(void *p_data) { Context::data = p_data; }
};

/// Completion port sub-service based on sese-event
class IOCPService final : public service::v2::TimerableService {
public:
    /// Initialize sub-service
    /// \param master Master server
    /// \param active_release_mode Active release mode
    explicit IOCPService(IOCPServer *master, bool active_release_mode);
    ~IOCPService() override;

    /**
     * Submit read event to the master server
     * @param ctx Operation context
     */
    void postRead(Context *ctx);
    /**
     * Submit write event to the master server
     * @param ctx Operation context
     */
    void postWrite(Context *ctx);
    /**
     * Submit close event to the master server
     * @param ctx Operation context
     */
    void postClose(Context *ctx);
    /**
     * Sub-server connection access completion callback function
     * @param ctx Operation context
     */
    static void onAcceptCompleted(Context *ctx);
    /**
     * Sub-service read event trigger callback function
     * @param ctx Operation context
     */
    static void onPreRead(Context *ctx);
    /**
     * Sub-service read event completion trigger callback function
     * @param ctx Operation context
     */
    static void onReadCompleted(Context *ctx);
    /**
     * Sub-service write event completion trigger callback function
     * @param ctx Operation context
     */
    static void onWriteCompleted(Context *ctx);
    /**
     * Sub-service timeout callback function
     * @param ctx Operation context
     */
    static void onTimeout(Context *ctx);
    /**
     * Connection event callback function
     * @param ctx Operation context
     */
    static void onConnected(Context *ctx);
    /**
     * ALPN negotiation completion callback function
     * @param ctx Operation context
     * @param in Negotiation content
     * @param in_length Negotiation content length
     */
    static void onAlpnGet(Context *ctx, const uint8_t *in, uint32_t in_length);
    /**
     * ALPN negotiation callback function
     * @param ssl SSL context
     * @param out Expected content from peer
     * @param out_length Length of expected content from peer
     * @param in Response content
     * @param in_length Length of response content
     * @param service Associated sub-service
     * @return ALPN status code
     */
    static int alpnCallbackFunction(
            void *ssl,
            const uint8_t **out, uint8_t *out_length,
            const uint8_t *in, uint32_t in_length,
            IOCPService *service
    );

private:
    void onAccept(int fd) override;
    void onRead(event::BaseEvent *event) override;
    void onWrite(event::BaseEvent *event) override;
    void onClose(event::BaseEvent *event) override;
    void onTimeout(service::v2::TimeoutEvent *event) override;

    event::BaseEvent *createEventEx(int fd, unsigned int events, void *data);
    void freeEventEx(sese::event::BaseEvent *event);

    void releaseContext(Context *ctx);

    static int64_t read(int fd, void *buffer, size_t len, void *ssl);
    static int64_t write(int fd, const void *buffer, size_t len, void *ssl);

    IOCPServer *master{};
    std::set<event::BaseEvent *> eventSet;
};

} // namespace sese::iocp::v1