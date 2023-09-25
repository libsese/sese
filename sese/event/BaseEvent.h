#pragma once

#define EVENT_NULL 0x0u
#define EVENT_READ 0x1u
#define EVENT_WRITE 0x2u
#define EVENT_ERROR 0x4u

namespace sese::event {
    struct BaseEvent {
        virtual ~BaseEvent() = default;

        int fd{EVENT_NULL};
        unsigned int events{0};
        void *data{nullptr};
    };
}// namespace sese::event