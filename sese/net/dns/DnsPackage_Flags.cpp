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

#include "DnsPackage.h"

void sese::net::dns::DnsPackage::Flags::decode(uint16_t flags) {
    qr = (flags >> 15) & 0x1;
    opcode = (flags >> 11) & 0xF;
    aa = (flags >> 10) & 0x1;
    tc = (flags >> 9) & 0x1;
    rd = (flags >> 8) & 0x1;
    ra = (flags >> 7) & 0x1;
    z = (flags >> 4) & 0x7;
    rcode = flags & 0xF;
}

uint16_t sese::net::dns::DnsPackage::Flags::encode() const {
    uint16_t flags = 0;

    flags |= (qr & 0x1) << 15;
    flags |= (opcode & 0xF) << 11;
    flags |= (aa & 0x1) << 10;
    flags |= (tc & 0x1) << 9;
    flags |= (rd & 0x1) << 8;
    flags |= (ra & 0x1) << 7;
    flags |= (z & 0x7) << 4;
    flags |= (rcode & 0xF);

    return flags;
}

