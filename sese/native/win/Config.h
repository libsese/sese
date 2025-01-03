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

/**
 * @file Config.h
 * @brief Windows Platform Configuration File
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#if !defined(_WIN32_WINNET) || _WIN32_WINNI < 0x0602
#define _WIN32_WINNNT 0x0602
#endif

#ifndef WINVER
#define WINVER 0x0602
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <windows.h>

#undef min
#undef max

#pragma warning(disable : 4819)

#if defined(_M_X64)
#define SESE_ARCH_X64
#elif defined(_M_ARM64)
#define SESE_ARCH_ARM64
#else
#error only support x86_64 & arm64
#endif

/// C++ version identifier
#define SESE_CXX_STANDARD _MSVC_LANG

/// Exportable Type Identifier - Now deprecated
#define API

/// pid_t Formatting placeholders
#define PRIdTid "u"

#ifndef __MINGW32__
/// Ignore case comparisons
#define strcasecmp strcmpi
#endif

#ifndef timegm
#define timegm _mkgmtime
#endif

#if defined(__MINGW32__)
#define SESE_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define SESE_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

namespace sese {
/// Process ID identifier
using pid_t = DWORD;
/// Thread ID identifier
using tid_t = uint32_t;
/// Native Socket type
using socket_t = SOCKET;
} // namespace sese

/// WSA Error Mapping

#undef EINTR
#undef EBADF
#undef EACCES
#undef EFAULT
#undef EINVAL
#undef EMFILE
#undef EWOULDBLOCK
#undef EINPROGRESS
#undef EALREADY
#undef ENOTSOCK
#undef EDESTADDRREQ
#undef EMSGSIZE
#undef EPROTOTYPE
#undef ENOPROTOOPT
#undef EPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#undef EOPNOTSUPP
#undef EPFNOSUPPORT
#undef EAFNOSUPPORT
#undef EADDRINUSE
#undef EADDRNOTAVAIL
#undef ENETDOWN
#undef ENETUNREACH
#undef ENETRESET
#undef ECONNABORTED
#undef ECONNRESET
#undef ENOBUFS
#undef EISCONN
#undef ENOTCONN
#undef ESHUTDOWN
#undef ETOOMANYREFS
#undef ETIMEDOUT
#undef ECONNREFUSED
#undef ELOOP
#undef ENAMETOOLONG
#undef EHOSTDOWN
#undef EHOSTUNREACH
#undef ENOTEMPTY
#undef EPROCLIM
#undef EUSERS
#undef EDQUOT
#undef ESTALE
#undef EREMOTE

#define EINTR WSAEINTR
#define EBADF WSAEBADF
#define EACCES WSAEACCES
#define EFAULT WSAEFAULT
#define EINVAL WSAEINVAL
#define EMFILE WSAEMFILE
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINPROGRESS WSAEINPROGRESS
#define EALREADY WSAEALREADY
#define ENOTSOCK WSAENOTSOCK
#define EDESTADDRREQ WSAEDESTADDRREQ
#define EMSGSIZE WSAEMSGSIZE
#define EPROTOTYPE WSAEPROTOTYPE
#define ENOPROTOOPT WSAENOPROTOOPT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#define EOPNOTSUPP WSAEOPNOTSUPP
#define EPFNOSUPPORT WSAEPFNOSUPPORT
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#define EADDRINUSE WSAEADDRINUSE
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define ENETDOWN WSAENETDOWN
#define ENETUNREACH WSAENETUNREACH
#define ENETRESET WSAENETRESET
#define ECONNABORTED WSAECONNABORTED
#define ECONNRESET WSAECONNRESET
#define ENOBUFS WSAENOBUFS
#define EISCONN WSAEISCONN
#define ENOTCONN WSAENOTCONN
#define ESHUTDOWN WSAESHUTDOWN
#define ETOOMANYREFS WSAETOOMANYREFS
#define ETIMEDOUT WSAETIMEDOUT
#define ECONNREFUSED WSAECONNREFUSED
#define ELOOP WSAELOOP
#define ENAMETOOLONG WSAENAMETOOLONG
#define EHOSTDOWN WSAEHOSTDOWN
#define EHOSTUNREACH WSAEHOSTUNREACH
#define ENOTEMPTY WSAENOTEMPTY
#define EPROCLIM WSAEPROCLIM
#define EUSERS WSAEUSERS
#define EDQUOT WSAEDQUOT
#define ESTALE WSAESTALE
#define EREMOTE WSAEREMOTE