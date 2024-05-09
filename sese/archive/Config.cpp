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

#include <sese/archive/Config.h>

#include <archive_entry.h>

using sese::archive::Config;

// GCOVR_EXCL_START

int Config::toValue(Config::EntryType type) {
#define XX(str)\
    case EntryType::str:  \
        return AE_IF##str;

    switch (type) {
        XX(MT)
        XX(REG)
        XX(LNK)
        XX(SOCK)
        XX(CHR)
        XX(BLK)
        XX(DIR)
        XX(IFO)
        default:
            return 0;
    }
#undef XX
}

Config::EntryType Config::fromValue(int value) {
#define XX(str) \
    case AE_IF##str: \
        return Config::EntryType::str;
    switch (value) {
        XX(MT)
        XX(REG)
        XX(LNK)
        XX(SOCK)
        XX(CHR)
        XX(BLK)
        XX(DIR)
        XX(IFO)
        default:
            return Config::EntryType::REG;
    }
#undef XX
}

// GCOVR_EXCL_STOP