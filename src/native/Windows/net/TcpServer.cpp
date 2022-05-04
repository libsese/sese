#include <sese/net/TcpServer.h>
#include <sese/system/CpuInfo.h>

using sese::CpuInfo;
using sese::IOContext;
using sese::IPAddress;
using sese::Socket;
using sese::TcpServer;
using sese::Thread;

IOContext::IOContext() noexcept {
    event = WSACreateEvent();
    overlapped.hEvent = event;
    wsaBuf.buf = buffer;
    wsaBuf.len = SERVER_MAX_BUFFER_SIZE;
    operationType = OperationType::Null;
}

IOContext::~IOContext() noexcept {
    WSACloseEvent(event);
}

int64_t IOContext::send() noexcept {
    DWORD nBytes = SERVER_MAX_BUFFER_SIZE;
    DWORD dwFlags = 0;
    // 实际传输字节数
    DWORD cbTransfer;
    operationType = OperationType::Send;
    int32_t nRet = WSASend(socket,
                           &wsaBuf,
                           1,
                           &nBytes,
                           dwFlags,
                           &overlapped,
                           nullptr);

    if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
        return -1;
    }

    WSAWaitForMultipleEvents(1, &event, FALSE, WSA_INFINITE, FALSE);
    WSAGetOverlappedResult(socket, &overlapped, &cbTransfer, FALSE, &dwFlags);
    WSAResetEvent(event);
    return cbTransfer;
}

int64_t IOContext::recv() noexcept {
    DWORD nBytes = SERVER_MAX_BUFFER_SIZE;
    DWORD dwFlags = 0;
    // 实际传输字节数
    DWORD cbTransfer;

    operationType = OperationType::Recv;
    int32_t nRet = WSARecv(socket,
                           &wsaBuf,
                           1,
                           &nBytes,
                           &dwFlags,
                           &overlapped,
                           nullptr);

    if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
        return -1;
    }

    WSAWaitForMultipleEvents(1, &event, FALSE, WSA_INFINITE, FALSE);
    WSAGetOverlappedResult(socket, &overlapped, &cbTransfer, FALSE, &dwFlags);
    WSAResetEvent(event);
    return cbTransfer;
}

void IOContext::close() noexcept {
    closesocket(socket);
}

bool TcpServer::init(const IPAddress::Ptr &ipAddress) noexcept {
    // IOCP 要求使用 WSASocket 创建 Socket 文件描述符
    auto family = ipAddress->getRawAddress()->sa_family;
    SOCKET serverSocket = WSASocketW(family, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (SOCKET_ERROR == serverSocket) {
        return false;
    }

    socket = std::make_shared<Socket>(serverSocket, ipAddress);
    if (SOCKET_ERROR == socket->bind(ipAddress)) {
        socket->close();
        return false;
    }

    if (SOCKET_ERROR == socket->listen(SERVER_MAX_CONNECTION)) {
        socket->close();
        return false;
    }

    // 创建 IOCP
    DWORD numberOfThreads = CpuInfo::getLogicProcessors();
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, numberOfThreads);
    if (INVALID_HANDLE_VALUE == iocpHandle) {
        socket->close();
        return false;
    }

    for (int32_t i = 0; i < numberOfThreads; i++) {
        threadGroup.emplace_back(
                std::make_shared<Thread>(
                        [this] { workerProc4WindowsIOCP(); },
                        "WindowsIOCPWorkerThread" + std::to_string(i)));
    }

    return true;
}

void TcpServer::workerProc4WindowsIOCP() {
    IOContext *ioContext = nullptr;
    DWORD nBytes = SERVER_MAX_BUFFER_SIZE;
    DWORD dwFlags = 0;
    DWORD dwContextBufferSize = 0;
    void *lpCompletionKey = nullptr;
    while (!isShutdown) {
        BOOL nRet = GetQueuedCompletionStatus(
                iocpHandle,
                &dwContextBufferSize,
                (PULONG_PTR) &lpCompletionKey,
                (LPOVERLAPPED *) &ioContext,
                INFINITE);

        if (dwContextBufferSize == 0) {
            break;
        }

        if (!nRet) {
            closesocket(ioContext->socket);
            delete ioContext;
            continue;
        }

        switch (ioContext->operationType) {
            case OperationType::Send:
                nRet = WSASend(
                        ioContext->socket,
                        &(ioContext->wsaBuf),
                        1,
                        &nBytes,
                        dwFlags,
                        &(ioContext->overlapped),
                        nullptr);
                if (nRet == SOCKET_ERROR && ERROR_IO_PENDING != WSAGetLastError()) {
                    closesocket(ioContext->socket);
                    delete ioContext;
                    continue;
                }
                break;
            case OperationType::Recv:
                nRet = WSARecv(
                        ioContext->socket,
                        &ioContext->wsaBuf,
                        1,
                        &nBytes,
                        &dwFlags,
                        &(ioContext->overlapped),
                        nullptr);
                if (nRet == SOCKET_ERROR && ERROR_IO_PENDING != WSAGetLastError()) {
                    closesocket(ioContext->socket);
                    delete ioContext;
                    continue;
                }
                break;
            case OperationType::Null:
                closesocket(ioContext->socket);
                delete ioContext;
                break;
        }
    }
}

void TcpServer::shutdown() {
    isShutdown = true;
    for (int32_t i = 0; i < threadGroup.size(); i++) {
        PostQueuedCompletionStatus(iocpHandle, 0, (DWORD) 0, nullptr);
    }
    for (const auto &th: threadGroup) {
        th->join();
    }
    CloseHandle(iocpHandle);
    socket->close();
}

void TcpServer::loopWith(const std::function<void(IOContext *)> &handler) {
    while (!isShutdown) {
        auto client = socket->accept();
        if (client == nullptr) {
            continue;
        }

        if (!client->setNonblocking(true)) {
            client->close();
            continue;
        }

        if (CreateIoCompletionPort((HANDLE) client->getRawSocket(), iocpHandle, 0, 0) == nullptr) {
            client->close();
            continue;
        }

        auto data = new IOContext;
        data->socket = client->getRawSocket();
        handler(data);
        delete data;
    }
}
