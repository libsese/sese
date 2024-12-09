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

#include <sese/io/InputStreamReader.h>

sese::io::InputStreamReader::InputStreamReader(sese::io::PeekableStream *input) noexcept : input(input), builder(2048) {
}

std::string sese::io::InputStreamReader::readLine() noexcept {
    builder.clear();
    // Indicates that this is an exit due to no content to continue reading,
    // and should be returned regardless of whether there is content in the builder
    bool end = false;
    char ch;
retry:
    while (true) {
        if (1 != input->peek(&ch, 1)) {
            end = true;
            break;
        }
        input->trunc(1);

        if (ch == '\r') {
            if (1 == input->peek(&ch, 1)) {
                if (ch == '\n') {
                    // \r\n
                    input->trunc(1);
                    break;
                } else {
                    // \r
                    break;
                }
            } else {
                end = true;
                break;
            }
        } else if (ch == '\n') {
            // \n
            break;
        } else {
            builder.append(ch);
        }
    }

    if (builder.empty() && !end) {
        // Blank lines retry
        goto retry;
    }

    return builder.toString();
}