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

#include <sese/system/Paths.h>

#ifndef SESE_PLATFORM_WINDOWS
#include <climits>
#include <unistd.h>
#endif

#ifdef SESE_PLATFORM_APPLE
#include <mach-o/dyld.h>
#endif

static sese::system::Path g_work_dir;

static sese::system::Path g_executable_path;

static std::string g_executable_name;

sese::system::PathsInitiateTask::PathsInitiateTask() : InitiateTask(__FUNCTION__) {}

int32_t sese::system::PathsInitiateTask::init() noexcept {
#ifdef SESE_PLATFORM_WINDOWS
#define MY_PATH_MAX 4096
    char buffer[MY_PATH_MAX];
    DWORD len;
    len = GetCurrentDirectoryA(sizeof(buffer), buffer);
    if (len == 0) return -1;
    Path::replaceWindowsPathSplitChar(buffer, len);
    g_work_dir = Path::fromNativePath(buffer);
    if (!g_work_dir.isValid()) return -1;

    // ZeroMemory(buffer, sizeof(buffer));
    memset(buffer, 0, sizeof(buffer));
    len = GetModuleFileNameA(nullptr, buffer, MY_PATH_MAX);
    if (len == 0) return -1;
    Path::replaceWindowsPathSplitChar(buffer, len);
    auto p = strrchr(buffer, '/');
    g_executable_path = Path::fromNativePath(buffer);
    *p = 0;
    p += 1;
    if (!g_executable_path.isValid()) return -1;
    g_executable_name = p;

    return 0;
#undef MY_PATH_MAX
#else
    char buffer[PATH_MAX]{};
    auto n = getcwd(buffer, sizeof(buffer));
    if (n == nullptr) return -1;
    g_work_dir = buffer;
    if (!g_work_dir.isValid()) return -1;

#ifdef SESE_PLATFORM_LINUX
    memset(buffer, 0 , sizeof(buffer));
    auto i = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (i == -1) return -1;
    g_executable_path = buffer;
    auto p = std::strrchr(buffer, '/');
    *p = 0;
    p += 1;
    g_executable_name = p;
#else
    memset(buffer, 0 , sizeof(buffer));
    uint32_t len = sizeof(buffer);
    if(_NSGetExecutablePath(buffer, &len) != 0) return -1;
    g_executable_path = buffer;
    auto p = std::strrchr(buffer, '/');
    *p = 0;
    p += 1;
    g_executable_name = p;
#endif

    return 0;
#endif
}

int32_t sese::system::PathsInitiateTask::destroy() noexcept {
    return 0;
}

const sese::system::Path &sese::system::Paths::getWorkDir() {
    return g_work_dir;
}

const sese::system::Path &sese::system::Paths::getExecutablePath() {
    return g_executable_path;
}

const std::string &sese::system::Paths::getExecutableName() {
    return g_executable_name;
}