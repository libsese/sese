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

#include "Bundler.h"
#include "sese/config/Json.h"
#include "sese/io/FileStream.h"
#include "sese/system/CommandLine.h"
#include "sese/util/ArgParser.h"
#include "sese/util/Exception.h"

#include <filesystem>

void Bundler::parse_config() {
    sese::ArgParser parser;
    if (!parser.parse(sese::system::CommandLine::getArgc(), sese::system::CommandLine::getArgv())) {
        throw sese::Exception("Failed to parse command line arguments");
    }

    if (!parser.exist("--base_path")) {
        throw sese::Exception("`base_path` not found in command line arguments");
    }
    base_path = parser.getValueByKey("--base_path");
    SESE_INFO("base_path = {}", base_path);

    if (!parser.exist("--resource_file_path")) {
        throw sese::Exception("`resource_file_path` not found in command line arguments");
    }
    resource_file_path = parser.getValueByKey("--resource_file_path");
    SESE_INFO("resource_file_path = {}", resource_file_path);

    if (!parser.exist("--generate_code_path")) {
        throw sese::Exception("`generate_code_path` not found in command line arguments");
    }
    generate_code_path = parser.getValueByKey("--generate_code_path");
    SESE_INFO("generate_code_path = {}", generate_code_path);

    if (!parser.exist("--method_name")) {
        throw sese::Exception("`method_name` not found in command line arguments");
    }
    method_name = parser.getValueByKey("--method_name");
    SESE_INFO("method_name = {}", method_name);

    class_name = std::filesystem::path(resource_file_path).stem().string();
}

void Bundler::parse_resource_file() {
    auto result = sese::io::File::createEx(resource_file_path, "rt");
    if (result) {
        throw sese::Exception("Failed to open resource file");
    }
    auto value = sese::Json::parse(result.get().get(), 4);
    auto &root = value.getDict();
    auto &binaries = root.find("binaries")->getDict();
    for (auto &[k, v]: binaries) {
        auto bin = base_path + "/" + v->getString();
        this->binaries[k] = bin;
        SESE_INFO("binaries[{}] = {}", k, bin);
    }
}

int Bundler::main() {
    try {
        Bundler bundler;
        bundler.parse_config();
        bundler.parse_resource_file();
        if (bundler.method_name == "rc") {
            bundler.write_rc_resource_file();
            bundler.write_rc_file();
        } else if (bundler.method_name == "ld") {
            bundler.write_elf_header_file();
            bundler.write_elf_source_file();
            bundler.make_elf_resources();
        } else if (bundler.method_name == "mash-o") {
            bundler.write_mach_o_header_file();
        } else {
            throw sese::Exception("Unknown method name");
        }
    } catch (sese::Exception &e) {
        e.printStacktrace();
    }
    return 0;
}

int main(int argc, char *argv[]) {
    sese::initCore(argc, argv);
    Bundler::main();
}
