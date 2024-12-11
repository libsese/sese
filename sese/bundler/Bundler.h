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

#pragma once

#include <sese/Log.h>

#include <string>
#include <map>

class Bundler {
public:
    static int main();

private:
    std::string base_path;
    std::string resource_file_path;
    std::string generate_code_path;

    std::string class_name;
    std::string method_name;
    std::map<std::string, std::string> binaries;

    void parse_config();
    void parse_resource_file();

    void write_rc_file();
    void write_rc_resource_file();

    static std::string sanitizeFilename(const std::string &name);

    void write_elf_header_file();
    void write_elf_source_file();
    void make_elf_resources();

    void write_mach_o_header_file();
};
