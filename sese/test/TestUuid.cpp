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

#include <sese/util/UuidBuilder.h>

#include <gtest/gtest.h>

TEST(TestUuid, Builder) {
    auto builder = sese::UuidBuilder();
    sese::Uuid id;
    ASSERT_TRUE(builder.generate(id));
}

TEST(TestUuid, Chrono) {
    std::time_t tm;
    std::time(&tm);
    std::cout << "std::time_t -> " << tm << "\n";

    auto point1 = std::chrono::high_resolution_clock::now();
    std::cout << "std::chrono::high_resolution_clock -> " << point1.time_since_epoch().count() << "\n";

    auto point2 = std::chrono::system_clock::now();
    std::cout << "std::chrono::system_clock -> " << point2.time_since_epoch().count() << "\n";

    auto point3 = std::chrono::steady_clock::now();
    std::cout << "std::chrono::steady_clock -> " << point3.time_since_epoch().count() << "\n";
}

/// \note Here is a simple call to the interface. To reproduce the handling of time callbacks,
/// you need to write a separate test program and manually make the operating system time callbacks,
/// as are other TimestampHandler-based features
TEST(TestUuid, TimestampHandler) {
    sese::TimestampHandler timestamp_handler(std::chrono::system_clock::now());

    auto p1 = timestamp_handler.getCurrentTimestamp();
    std::cout << "p1 -> " << p1 << "\n";

    for (int i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        auto p2 = timestamp_handler.tryGetCurrentTimestamp();
        std::cout << "p2 -> " << p2 << "\n";
    }
}

TEST(TestUuid, UUID) {
    uint8_t self_id = 0x23;
    sese::TimestampHandler timestamp_handler(std::chrono::system_clock::now());

    auto uuid0 = sese::Uuid(self_id, timestamp_handler.getCurrentTimestamp(), 0x45);
    uuid0.setSelfId(self_id);
    uuid0.setR(0x45);

    auto number = uuid0.toNumber();

    auto uuid1 = sese::Uuid();
    uuid1.parse(number);
    std::cout << "uuid0.selfId -> " << uuid0.getSelfId() << "\n";
    std::cout << "uuid1.selfId -> " << uuid1.getSelfId() << "\n";
    std::cout << "uuid0.r -> " << uuid0.getR() << "\n";
    std::cout << "uuid1.r -> " << uuid1.getR() << "\n";
    std::cout << "uuid0.timestamp -> " << uuid0.getTimestamp() << "\n";
    std::cout << "uuid1.timestamp -> " << uuid1.getTimestamp() << "\n";
}