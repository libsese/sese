#pragma one

#include <sese/Config.h>
#include <sese/config/json/JsonTypes.h>
#include <sese/security/SSLContext.h>
#include <sese/security/SecuritySocket.h>
#include <sese/service/TimerableService.h>
#include <sese/util/ByteBuilder.h>

#include <map>
#include <functional>

namespace sese::service {

    class API RpcService final : public TimerableService {
    public:
        using Func = std::function<void(json::ObjectData::Ptr &, json::ObjectData::Ptr &)>;

        explicit RpcService(security::SSLContext::Ptr context, uint64_t timeout) noexcept;

        void setFunction(const std::string &name, const Func &func) noexcept;

        void onAccept(int fd) override;

        void onRead(event::BaseEvent *event) override;

        void onWrite(event::BaseEvent *event) override;

        void onClose(event::BaseEvent *event) override;

        void onTimeout(TimeoutEvent *timeoutEvent) override;

    private:
        static int64_t read(int fd, void *buffer, size_t len, void *ssl) noexcept;

        static int64_t write(int fd, const void *buffer, size_t len, void *ssl) noexcept;

        bool onHandle(ByteBuilder *builder);

        security::SSLContext::Ptr context;
        uint64_t timeout;

        std::map<int, ByteBuilder *> buffers;
        std::map<std::string, Func> funcs;
    };
}// namespace sese::service