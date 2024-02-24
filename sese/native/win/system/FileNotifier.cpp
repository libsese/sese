#include "sese/system/FileNotifier.h"

#include <assert.h>
#include <windows.h>
#include <locale>
#include <codecvt>

#pragma warning(disable : 4996)

using namespace sese::system;

FileNotifier::Ptr FileNotifier::create(const std::string &path, FileNotifyOption *option) noexcept {
    const auto FILE_HANDLE = CreateFile(
        path.c_str(),
        GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr
    );
    if (FILE_HANDLE == INVALID_HANDLE_VALUE) { return nullptr; }

    const auto overlapped = new OVERLAPPED{};
    overlapped->hEvent = CreateEventA(nullptr, false, false, nullptr);

    const auto NOTIFIER = new FileNotifier;
    NOTIFIER->fileHandle = FILE_HANDLE;
    NOTIFIER->overlapped = overlapped;
    NOTIFIER->option = option;

    return std::unique_ptr<FileNotifier>(NOTIFIER);
}

FileNotifier::~FileNotifier() noexcept { if (this->th) { shutdown(); } }

void FileNotifier::loopNonblocking() noexcept {
    auto proc = [this]() {
        std::wstring_convert<std::codecvt_utf8<wchar_t> > convert;
        DWORD read = 0;
        while (!isShutdown) {
            char buffer[1024];
            if (!ReadDirectoryChangesW(
                (HANDLE) fileHandle,
                buffer,
                sizeof(buffer),
                false,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
                (LPDWORD) &read,
                static_cast<LPOVERLAPPED>(overlapped),
                nullptr
            )) { break; }
            if (WaitForSingleObject(static_cast<LPOVERLAPPED>(overlapped)->hEvent, INFINITE) == WAIT_OBJECT_0) {
                if (!isShutdown) {
                    size_t count = 0;
                    auto info_pos = buffer + 0;
                    std::string name0;
                    std::string name1;
                again:
                    const auto INFO = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(info_pos);
                    if (count % 2) {
                        name1 = convert.to_bytes(
                            INFO->FileName,
                            reinterpret_cast<wchar_t *>(reinterpret_cast<char *>(INFO->FileName) + INFO->FileNameLength)
                        );
                    } else {
                        name0 = convert.to_bytes(
                            INFO->FileName,
                            reinterpret_cast<wchar_t *>(reinterpret_cast<char *>(INFO->FileName) + INFO->FileNameLength)
                        );
                    }

                    switch (INFO->Action) {
                        case FILE_ACTION_ADDED:
                            option->onCreate(name0);
                            break;
                        case FILE_ACTION_MODIFIED:
                            option->onModify(name0);
                            break;
                        case FILE_ACTION_REMOVED:
                            option->onDelete(name0);
                            break;
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            count += 1;
                            info_pos += INFO->NextEntryOffset;
                            goto again;
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            count += 1;
                            option->onMove(name0, name1);
                            break;
                        default:
                            assert(false);
                            break;
                    }
                } else { break; }
            }
        }
    };
    th = std::make_unique<Thread>(proc);
    th->start();
}

void FileNotifier::shutdown() noexcept {
    isShutdown = true;
    SetEvent(static_cast<LPOVERLAPPED>(overlapped)->hEvent);
    th->join();
    th = nullptr;
    CloseHandle(fileHandle);
    fileHandle = nullptr;
    delete static_cast<LPOVERLAPPED>(overlapped);
}