#include "sese/system/FileNotifier.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <tuple>

static void callback(
        ConstFSEventStreamRef stream,
        void *info,
        size_t num_events,
        void *event_paths,
        const FSEventStreamEventFlags event_flags[],
        const FSEventStreamEventId event_ids[]
) {
    auto option = (sese::system::FileNotifyOption *) info;
    const char *last_src = nullptr;
    for (size_t i = 0; i < num_events; ++i) {
        auto dict = (CFDictionaryRef) CFArrayGetValueAtIndex((CFArrayRef) event_paths, (CFIndex) i);
        auto path = (CFStringRef) CFDictionaryGetValue(dict, kFSEventStreamEventExtendedDataPathKey);
        auto src = CFStringGetCStringPtr(path, kCFStringEncodingUTF8);

        if (event_flags[i] & kFSEventStreamEventFlagItemCreated) {
            auto len = std::string_view(src).find_last_of('/') + 1;
            option->onCreate(src + len);
        }
        if (event_flags[i] & kFSEventStreamEventFlagItemModified) {
            auto len = std::string_view(src).find_last_of('/') + 1;
            option->onModify(src + len);
        }
        if (event_flags[i] & kFSEventStreamEventFlagItemRenamed) {
            if (last_src) {
                auto len0 = std::string_view(last_src).find_last_of('/') + 1;
                auto len1 = std::string_view(src).find_last_of('/') + 1;
                option->onMove(last_src + len0, src + len1);
                last_src = nullptr;
            } else {
                last_src = src;
            }
        }
        if (event_flags[i] & kFSEventStreamEventFlagItemRemoved) {
            auto len = std::string_view(src).find_last_of('/') + 1;
            option->onDelete(src + len);
        }
    }
}

using namespace sese::system;

FileNotifier::Ptr FileNotifier::create(const std::string &path, FileNotifyOption *option) noexcept {
    CFStringRef p = CFStringCreateWithCString(
            nullptr,
            path.c_str(),
            kCFStringEncodingUTF8
    );

    CFArrayRef paths_to_watch = CFArrayCreate(
            nullptr,
            (const void **) &p,
            1,
            &kCFTypeArrayCallBacks
    );

    auto notifier = new FileNotifier;

    FSEventStreamContext context;
    context.version = 0;
    context.info = option;
    context.retain = nullptr;
    context.release = nullptr;
    context.copyDescription = nullptr;

    notifier->stream = FSEventStreamCreate(
            nullptr,
            &callback,
            &context,
            paths_to_watch,
            kFSEventStreamEventIdSinceNow,
            1,
            kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagUseCFTypes | kFSEventStreamCreateFlagUseExtendedData
    );

    notifier->queue = dispatch_queue_create(nullptr, nullptr);
    FSEventStreamSetDispatchQueue(
            (FSEventStreamRef) notifier->stream,
            (dispatch_queue_t) notifier->queue
    );

    return std::unique_ptr<FileNotifier>(notifier);
}

FileNotifier::~FileNotifier() noexcept {
    if (this->stream) {
        shutdown();
    }
}

void FileNotifier::loopNonblocking() noexcept {
    FSEventStreamStart((FSEventStreamRef) this->stream);
}

void FileNotifier::shutdown() noexcept {
    FSEventStreamStop((FSEventStreamRef) this->stream);
    FSEventStreamInvalidate((FSEventStreamRef) this->stream);
    FSEventStreamRelease((FSEventStreamRef) this->stream);
    dispatch_release((dispatch_queue_t) this->queue);
    stream = nullptr;
    queue = nullptr;
}