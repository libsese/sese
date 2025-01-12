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

#include "sese/system/FileNotifier.h"
#include "sese/util/Memory.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

using namespace sese::system;

class FileNotifier::Impl {
public:
    using Ptr = std::unique_ptr<Impl>;

    FSEventStreamRef stream = nullptr;
    dispatch_queue_t queue = nullptr;

    OnCreateCallback on_create;
    OnMoveCallback on_move;
    OnModifyCallback on_modify;
    OnDeleteCallback on_delete;

    static void callback(
        ConstFSEventStreamRef stream,
        void *this__,
        size_t num_events,
        void *event_paths,
        const FSEventStreamEventFlags event_flags[],
        const FSEventStreamEventId event_ids[]
    ) {
        auto this_ = (FileNotifier::Impl *) this__;
        const char *last_src = nullptr;
        for (size_t i = 0; i < num_events; ++i) {
            auto dict = (CFDictionaryRef) CFArrayGetValueAtIndex((CFArrayRef) event_paths, (CFIndex) i);
            auto path = (CFStringRef) CFDictionaryGetValue(dict, kFSEventStreamEventExtendedDataPathKey);
            auto src = CFStringGetCStringPtr(path, kCFStringEncodingUTF8);

            if (event_flags[i] & kFSEventStreamEventFlagItemCreated) {
                auto len = std::string_view(src).find_last_of('/') + 1;
                if (this_->on_create) {
                    this_->on_create(src + len);
                }
            }
            if (event_flags[i] & kFSEventStreamEventFlagItemModified) {
                auto len = std::string_view(src).find_last_of('/') + 1;
                if (this_->on_modify) {
                    this_->on_modify(src + len);
                }
            }
            if (event_flags[i] & kFSEventStreamEventFlagItemRenamed) {
                if (last_src) {
                    auto len0 = std::string_view(last_src).find_last_of('/') + 1;
                    auto len1 = std::string_view(src).find_last_of('/') + 1;
                    if (this_->on_move) {
                        this_->on_move(last_src + len0, src + len1);
                    }
                    last_src = nullptr;
                } else {
                    last_src = src;
                }
            }
            if (event_flags[i] & kFSEventStreamEventFlagItemRemoved) {
                auto len = std::string_view(src).find_last_of('/') + 1;
                if (this_->on_delete) {
                    this_->on_delete(src + len);
                }
            }
        }
    }


    static Ptr create(const std::string &path) noexcept {
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

        auto impl = MAKE_UNIQUE_PRIVATE(Impl);

        FSEventStreamContext context;
        context.version = 0;
        context.info = impl.get();
        context.retain = nullptr;
        context.release = nullptr;
        context.copyDescription = nullptr;

        impl->stream = FSEventStreamCreate(
            nullptr,
            &callback,
            &context,
            paths_to_watch,
            kFSEventStreamEventIdSinceNow,
            1,
            kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagUseCFTypes | kFSEventStreamCreateFlagUseExtendedData
        );

        impl->queue = dispatch_queue_create(nullptr, nullptr);
        FSEventStreamSetDispatchQueue(
            impl->stream,
            impl->queue
        );

        return impl;
    }

    ~Impl() noexcept {
        if (this->stream) {
            shutdown();
        }
    }

    void start() noexcept {
        FSEventStreamStart(this->stream);
    }

    void shutdown() noexcept {
        FSEventStreamStop(this->stream);
        FSEventStreamInvalidate(this->stream);
        FSEventStreamRelease(this->stream);
        dispatch_release(this->queue);
        stream = nullptr;
        queue = nullptr;
    }
};

FileNotifier::Ptr FileNotifier::create(const std::string &path) noexcept {
    auto result = MAKE_UNIQUE_PRIVATE(FileNotifier);
    auto impl = Impl::create(path);
    if (result->impl = std::move(impl); !result->impl) {
        return nullptr;
    }
    return result;
}

FileNotifier::~FileNotifier() noexcept {
}

void FileNotifier::start() const noexcept {
    return impl->start();
}

void FileNotifier::shutdown() const noexcept {
    return impl->shutdown();
}

void FileNotifier::setOnCreate(OnCreateCallback &&callback) const noexcept {
    impl->on_create = std::move(callback);
}

void FileNotifier::setOnMove(OnMoveCallback &&callback) const noexcept {
    impl->on_move = std::move(callback);
}

void FileNotifier::setOnModify(OnModifyCallback &&callback) const noexcept {
    impl->on_modify = std::move(callback);
}

void FileNotifier::setOnDelete(OnDeleteCallback &&callback) const noexcept {
    impl->on_delete = std::move(callback);
}
