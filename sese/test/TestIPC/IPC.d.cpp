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

#define SESE_C_LIKE_FORMAT

#include <sese/system/IPC.h>
#include <sese/record/Marco.h>
#include <sese/util/Initializer.h>
#include <sese/util/Util.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    auto channel = sese::system::IPCChannel::use("IPCTestForSese");
    while (true) {
        auto messages = channel->read(1);
        if (messages.empty()) {
            sese::sleep(1s);
            continue;
        }
        for (auto &&msg: messages) {
            SESE_INFO("recv %s", msg.getDataAsString().c_str());

            if (msg.getDataAsString() == "Exit") {
                goto end;
            }
        }
    }
end:
    return 0;
}