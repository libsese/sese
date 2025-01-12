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

#include <sese/util/Initializer.h>
#include <sese/log/Marco.h>
#include <sese/io/File.h>
#include <sese/config/Json.h>
#include <sese/config/Yaml.h>

int main(int argc, char *argv[]) {
    sese::initCore(argc, argv);
    SESE_INFO("Initialization is complete");

    if (argc < 2) {
        SESE_ERROR("Missing necessary command-line arguments");
        SESE_INFO("Usage: {} <json file>", argv[0]);
        return -1;
    }

    std::string src = argv[1];
    auto src_file = sese::io::File::create(src, sese::io::File::B_READ);
    if (src_file == nullptr) {
        SESE_ERROR("File {} failed to open", src);
        return -1;
    }

    auto json = sese::Json::parse(src_file.get());
    src_file->close();
    if (json.isNull()) {
        SESE_ERROR("Parsing {} failed", src);
        return -1;
    }

    auto dest = src + ".yml";
    auto dest_file = sese::io::File::create(dest, sese::io::File::B_WRITE_TRUNC);
    if (dest_file == nullptr) {
        SESE_ERROR("File {} failed to open", dest);
        return -1;
    }

    sese::Yaml::streamify(dest_file.get(), json);
    dest_file->close();
    SESE_INFO("Saved to {}", dest);

    return 0;
}