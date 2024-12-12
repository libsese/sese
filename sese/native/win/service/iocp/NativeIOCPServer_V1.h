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
 * @file NativeIOCPServer_V1.h
 * @brief Native IOCP Server for Windows
 * @author kaoru
 * @version 0.1
 * @date September 25, 2023
 */

#pragma once

#include <sese/net/Address.h>
#include <sese/security/SSLContext.h>
#include <sese/service/iocp/IOBuf.h>
#include <sese/thread/Thread.h>
#include <sese/io/ByteBuilder.h>
#include <sese/util/TimeWheel.h>

#include <atomic>
#include <set>
#include <mutex>

namespace sese::_windows::iocp::v1 {

class NativeIOCPServer;
struct OverlappedWrapper;

/// Native IOCP operational context
class NativeContext final : public io::InputStream, public io::OutputStream, public io::PeekableStream {
    friend class NativeIOCPServer;
    using IOBuf = sese::iocp::IOBuf;
    using Node = std::unique_ptr<sese::iocp::IOBufNode>;

    enum class Type {
        READ,
        WRITE,
        CONNECT,
        READY,
        CLOSE
    };

    OverlappedWrapper *pWrapper{};
    WSABUF wsabufRead{};
    WSABUF wsabufWrite{};
    Type type{Type::READ};
    SOCKET fd{INVALID_SOCKET};
    NativeIOCPServer *self{};
    TimeoutEvent *timeoutEvent{};
    void *ssl{};
    void *bio{};
    Node readNode;
    IOBuf recv{};
    io::ByteBuilder send{IOCP_WSABUF_SIZE, IOCP_WSABUF_SIZE};
    void *data{};

public:
    /**
     * Context initialization
     * @param p_wrapper Overlapped wrapper
     */
    explicit NativeContext(OverlappedWrapper *p_wrapper);
    ~NativeContext() override;
    /**
     * Read content from the current connection
     * @param buffer The buffer
     * @param length The size of the buffer
     * @return The actual amount of data read
     */
    int64_t read(void *buffer, size_t length) override;
    /**
     * Write content to the current connection
     * @param buffer The buffer
     * @param length The size of the buffer
     * @return The actual amount of data written
     */
    int64_t write(const void *buffer, size_t length) override;
    /**
     * Read content from the current connection without advancing
     * @param buffer The buffer
     * @param length The size of the buffer
     * @return The actual amount of data read
     */
    int64_t peek(void *buffer, size_t length) override;
    /**
     * Advance in the current connection without reading content
     * @param length The amount to advance
     * @return The actual amount advanced
     */
    int64_t trunc(size_t length) override;
    /**
     * Get the file descriptor of the current context connection
     * @return The file descriptor
     */
    [[nodiscard]] int32_t getFd() const { return static_cast<int32_t>(NativeContext::fd); }
    /**
     * Get the additional data of the current context
     * @return The additional data
     */
    [[nodiscard]] void *getData() const { return NativeContext::data; }
    /**
     * Set the additional data of the current context
     * @param p_data The additional data
     */
    void setData(void *p_data) { NativeContext::data = p_data; }
};

/// Overlapped Wrapper
struct OverlappedWrapper final {
    OVERLAPPED overlapped{};
    NativeContext ctx;

    OverlappedWrapper();
};

/// Native IOCP Server for Windows
class NativeIOCPServer {
public:
    using Context = NativeContext;
    using DeleteContextCallback = std::function<void(Context *data)>;

    virtual ~NativeIOCPServer() = default;

    /**
     * Initialize and start the server
     * @return The result of initialization
     */
    bool init();
    /**
     * Terminate worker threads, release system resources, and shut down the server
     */
    void shutdown();
    /**
     * Post a read event
     * @param ctx The operation context
     */
    void postRead(Context *ctx);
    /**
     * Post a write event
     * @param ctx The operation context
     */
    void postWrite(Context *ctx);
    /**
     * Post a close event
     * @param ctx The operation context
     */
    void postClose(Context *ctx);
    /**
     * Post a connection event
     * @param to The connection address
     * @param cli_ctx The SSL client context
     * @param data The additional data
     */
    void postConnect(const net::IPAddress::Ptr &to, const security::SSLContext::Ptr &cli_ctx, void *data = nullptr);
    /**
     * Set a timeout event
     * @param ctx The operation context
     * @param seconds The timeout duration
     */
    void setTimeout(Context *ctx, int64_t seconds);
    /**
     * Cancel a timeout event
     * @param ctx The operation context
     */
    void cancelTimeout(Context *ctx);
    /**
     * Default context release callback function
     */
    static void onDeleteContext(Context *) {}
    /**
     * Connection handshake completion callback function
     * @param ctx The operation context
     */
    virtual void onAcceptCompleted(Context *ctx) {}
    /**
     * Read event trigger callback function
     * @param ctx The operation context
     */
    virtual void onPreRead(Context *ctx) {}
    /**
     * Read event completion callback function
     * @param ctx The operation context
     */
    virtual void onReadCompleted(Context *ctx) {}
    /**
     * Write event completion callback function
     * @param ctx The operation context
     */
    virtual void onWriteCompleted(Context *ctx) {}
    /**
     * Timeout event callback function
     * @param ctx Operation context
     */
    virtual void onTimeout(Context *ctx) {}
    /**
     * Pre-connection event callback function
     * @param ctx The operation context
     */
    virtual void onPreConnect(Context *ctx) {}
    /**
     * Connection event callback function
     * @param ctx The operation context
     */
    virtual void onConnected(Context *ctx){};
    /**
     * ALPN protocol negotiation completion callback function
     * @param ctx The context
     * @param in The negotiation content
     * @param in_length The length of the negotiation content
     */
    virtual void onAlpnGet(Context *ctx, const uint8_t *in, uint32_t in_length){};
    /**
     * ALPN negotiation callback function
     * @param out The expected content from the peer
     * @param out_length The length of the expected content from the peer
     * @param in The response content
     * @param in_length The length of the response content
     * @return The ALPN status code
     */
    int onAlpnSelect(
            const uint8_t **out, uint8_t *out_length,
            const uint8_t *in, uint32_t in_length
    );

public:
    /**
     * Set the IP address bound to the current service. By setting this option, the server will automatically listen on the corresponding address's port.
     * @param addr Target IP
     */
    void setAddress(const net::IPAddress::Ptr &addr) { NativeIOCPServer::address = addr; }
    /**
     * Set the expected number of threads for the service
     * @param number_of_threads Number of threads
     */
    void setThreads(size_t number_of_threads) { NativeIOCPServer::threads = number_of_threads; }
    /**
     * Set the SSL context used for server listening
     * @param ctx SSL context
     */
    void setServCtx(const security::SSLContext::Ptr &ctx) { NativeIOCPServer::sslCtx = ctx; }
    /**
     * Set the ALPN protocol negotiation content for the server
     * @param protos Protocol negotiation content
     */
    void setServProtos(const std::string &protos) { NativeIOCPServer::servProtos = protos; }
    /**
     * Set the ALPN protocol negotiation content for the client
     * @param protos Protocol negotiation content
     */
    void setClientProtos(const std::string &protos) { NativeIOCPServer::clientProtos = protos; }
    /**
     * Set the server operation context destruction callback function
     * @param callback Callback function
     */
    void setDeleteContextCallback(const DeleteContextCallback &callback) { NativeIOCPServer::deleteContextCallback = callback; }
    /**
     * Get the current server listening SSL context
     * @return SSL context
     */
    [[nodiscard]] const security::SSLContext::Ptr &getServCtx() const { return NativeIOCPServer::sslCtx; }
    /**
     * Get the current service operation context destruction callback function
     * @return Callback function
     */
    [[nodiscard]] const DeleteContextCallback &getDeleteContextCallback() const { return NativeIOCPServer::deleteContextCallback; };
    /**
     * Get the active release mode status
     * @return Active release mode status
     */
    bool isActiveReleaseMode() const { return NativeIOCPServer::activeReleaseMode; }

protected:
    /**
     * Set the active release mode
     * @param enable Whether to enable
     */
    void setActiveReleaseMode(bool enable) { NativeIOCPServer::activeReleaseMode = enable; }
    /**
     * Release the operation context
     * @param ctx The operation context to be released
     */
    void releaseContext(Context *ctx);

    void acceptThreadProc();
    void eventThreadProc();
    static int alpnCallbackFunction(
            void *ssl,
            const uint8_t **out, uint8_t *out_length,
            const uint8_t *in, uint32_t in_length,
            NativeIOCPServer *server
    );

    void *connectEx{};
    bool initConnectEx();

    static long bioCtrl(void *bio, int cmd, long num, void *ptr);
    static int bioWrite(void *bio, const char *in, int length);
    static int bioRead(void *bio, char *out, int length);

    std::atomic_bool isShutdown{false};
    HANDLE iocpFd{INVALID_HANDLE_VALUE};
    SOCKET listenFd{INVALID_SOCKET};
    net::IPAddress::Ptr address{};
    Thread::Ptr acceptThread{};

    TimeWheel wheel{};
    std::set<OverlappedWrapper *> wrapperSet{};
    std::mutex wrapperSetMutex{};

    size_t threads{2};
    std::vector<Thread::Ptr> eventThreadGroup{};
    DeleteContextCallback deleteContextCallback = onDeleteContext;
    security::SSLContext::Ptr sslCtx{};
    void *bioMethod{};
    std::string servProtos{};
    std::string clientProtos{};

private:
    bool activeReleaseMode = true;
};

} // namespace sese::_windows::iocp::v1