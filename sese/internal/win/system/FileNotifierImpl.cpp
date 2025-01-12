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
#include "sese/util/EncodingConverter.h"
#include "sese/thread/Thread.h"

#include <windows.h>

#pragma warning(disable : 4996)

using namespace sese::system;

class FileNotifier::Impl {
public:
    void *file_handle = nullptr;
    void *overlapped = nullptr;
    std::atomic_bool is_shutdown = false;
    Thread::Ptr th = nullptr;

    OnCreateCallback on_create;
    OnMoveCallback on_move;
    OnModifyCallback on_modify;
    OnDeleteCallback on_delete;

    static std::unique_ptr<Impl> create(const std::string &path) noexcept {
        const auto FILE_HANDLE = CreateFile(
            path.c_str(),
            GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr
        );
        if (FILE_HANDLE == INVALID_HANDLE_VALUE) {
            return nullptr;
        }

        const auto overlapped = new OVERLAPPED{};
        overlapped->hEvent = CreateEventA(nullptr, false, false, nullptr);

        auto impl = std::make_unique<Impl>();
        impl->file_handle = FILE_HANDLE;
        impl->overlapped = overlapped;

        return impl;
    }

    ~Impl() noexcept {
        if (this->th) {
            shutdown();
        }
    }

    void shutdown() {
        is_shutdown = true;
        SetEvent(static_cast<LPOVERLAPPED>(overlapped)->hEvent);
        th->join();
        th = nullptr;
        CloseHandle(file_handle);
        file_handle = nullptr;
        delete static_cast<LPOVERLAPPED>(overlapped);
    }

    void start() {
        auto proc = [this] {
            DWORD read = 0;
            while (!is_shutdown) {
                char buffer[1024];
                if (!ReadDirectoryChangesW(
                        file_handle,
                        buffer,
                        sizeof(buffer),
                        false,
                        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
                        &read,
                        static_cast<LPOVERLAPPED>(overlapped),
                        nullptr
                    )) {
                    break;
                }
                if (WaitForSingleObject(static_cast<LPOVERLAPPED>(overlapped)->hEvent, INFINITE) == WAIT_OBJECT_0) {
                    if (!is_shutdown) {
                        size_t count = 0;
                        auto info_pos = buffer + 0;
                        std::string name0;
                        std::string name1;
                    again:
                        const auto INFO = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(info_pos);
                        if (count % 2) {
                            std::wstring_view range = {
                                INFO->FileName,
                                INFO->FileNameLength / sizeof(wchar_t)
                            };
                            name1 = EncodingConverter::toString(std::wstring(range));
                        } else {
                            std::wstring_view range = {
                                INFO->FileName,
                                INFO->FileNameLength / sizeof(wchar_t)
                            };
                            name0 = EncodingConverter::toString(std::wstring(range));
                        }

                        switch (INFO->Action) {
                            case FILE_ACTION_ADDED:
                                if (on_create) {
                                    on_create(name0);
                                }
                                break;
                            case FILE_ACTION_MODIFIED:
                                if (on_modify) {
                                    on_modify(name0);
                                }
                                break;
                            case FILE_ACTION_REMOVED:
                                if (on_delete) {
                                    on_delete(name0);
                                }
                                break;
                            case FILE_ACTION_RENAMED_OLD_NAME:
                                count += 1;
                                info_pos += INFO->NextEntryOffset;
                                goto again;
                            case FILE_ACTION_RENAMED_NEW_NAME:
                                count += 1;
                                if (on_move) {
                                    on_move(name0, name1);
                                }
                                break;
                            default:
                                assert(false);
                                break;
                        }
                    } else {
                        break;
                    }
                }
            }
        };
        th = std::make_unique<Thread>(proc);
        th->start();
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