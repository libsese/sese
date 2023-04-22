#include "sese/system/FileNotifier.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <tuple>

static void callback(
        ConstFSEventStreamRef stream,
        void *info,
        size_t numEvents,
        void *eventPaths,
        const FSEventStreamEventFlags eventFlags[],
        const FSEventStreamEventId eventIds[]
) {
    auto tuple = *(std::tuple<size_t, sese::FileNotifyOption *> *) info;
    auto len = std::get<0>(tuple) + 1;
    auto option = (sese::FileNotifyOption *) std::get<1>(tuple);
    const char *lastSrc = nullptr;
    for (size_t i = 0; i < numEvents; ++i) {
        auto dict = (CFDictionaryRef) CFArrayGetValueAtIndex((CFArrayRef) eventPaths, (CFIndex) i);
        auto path = (CFStringRef) CFDictionaryGetValue(dict, kFSEventStreamEventExtendedDataPathKey);
        auto src = CFStringGetCStringPtr(path, kCFStringEncodingUTF8);
        if (eventFlags[i] & kFSEventStreamEventFlagItemRemoved) {
            option->onDelete(src + len);
        } else if (eventFlags[i] & kFSEventStreamEventFlagItemCreated) {
            option->onCreate(src + len);
        } else if (eventFlags[i] & kFSEventStreamEventFlagItemModified) {
            option->onModify(src + len);
        } else if (eventFlags[i] & kFSEventStreamEventFlagItemRenamed) {
            if (lastSrc) {
                option->onMove(lastSrc + len, src + len);
                lastSrc = nullptr;
            } else {
                lastSrc = src;
            }
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

    CFArrayRef pathsToWatch = CFArrayCreate(
            nullptr,
            (const void **) &p,
            1,
            &kCFTypeArrayCallBacks
    );

    auto notifier = new FileNotifier;
    notifier->lenAndOption = std::tuple<size_t, FileNotifyOption *>(path.length(), option);

    FSEventStreamContext context;
    context.version = 0;
    context.info = &notifier->lenAndOption;
    context.retain = nullptr;
    context.release = nullptr;
    context.copyDescription = nullptr;

    notifier->stream = FSEventStreamCreate(
            nullptr,
            &callback,
            &context,
            pathsToWatch,
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