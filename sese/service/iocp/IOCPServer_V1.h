#pragma once

#include <sese/Config.h>
#include <sese/service/BalanceLoader.h>
#include <sese/service/TimerableService_V2.h>
#include <sese/io/ByteBuilder.h>

namespace sese::iocp {

    struct Context_V1;
    class IOCPServer_V1;
    class IOCPService_V1;

    class IOCPServer_V1 {
    public:
        using Context = Context_V1;

        static void postRead(Context *ctx);
        static void postWrite(Context *ctx);
        static void setTimeout(Context *ctx, int64_t seconds);
        static void cancelTimeout(Context *ctx);
        virtual void onReadCompleted(Context *ctx){};
        virtual void onWriteCompleted(Context *ctx){};
        virtual void onTimeout(Context *ctx){};
        virtual void onAlpnGet(const uint8_t *in, uint32_t inLength){};
        int onAlpnSelect(
                const uint8_t **out, uint8_t *outLength,
                const uint8_t *in, uint32_t inLength
        );

    protected:
        std::string servProtos {};
    };

    struct Context_V1 {
        IOCPServer_V1 *self{};
        IOCPService_V1 *client{};
        socket_t fd{0};
        event::BaseEvent *event{};
        service::TimeoutEvent_V2 *timeoutEvent{};
        io::ByteBuilder send{8192, 8192};
        io::ByteBuilder recv{8192, 8192};
        void *data{};
    };

    class IOCPService_V1 final : public service::TimerableService_V2 {
    public:
        using Context = Context_V1;

        explicit IOCPService_V1(IOCPServer_V1 *master);

        void postRead(Context *ctx);
        void postWrite(Context *ctx);
        static void onReadCompleted(Context *ctx);
        static void onWriteCompleted(Context *ctx);
        static void onTimeout(Context *ctx);
        static int alpnCallbackFunction(
                void *ssl,
                const uint8_t **out, uint8_t *outLength,
                const uint8_t *in, uint32_t inLength,
                IOCPService_V1 *service
        );

        void onAccept(int fd) override;
        void onRead(event::BaseEvent *event) override;
        void onWrite(event::BaseEvent *event) override;
        void onTimeout(service::TimeoutEvent_V2 *event) override;

    private:
        IOCPServer_V1 *master{};
    };

}// namespace sese::iocp