#include "Bundler.h"

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
}

void Bundler::parse_resource_file() {
    auto result = sese::io::File::createEx(resource_file_path, "rt");
    if (result) {
        throw sese::Exception("Failed to open resource file");
    }
    auto value = sese::Json::parse(result.get().get(), 4);
    auto &root = value.getDict();
    class_name = root.find("class")->getString();
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
#ifdef SESE_PLATFORM_WINDOWS
        bundler.write_rc_resource_file();
        bundler.write_rc_file();
#else
        bundler.write_ld_header_file();
        bundler.write_ld_source_file();
        bundler.make_ld_resources();
#endif
    } catch (sese::Exception &e) {
        e.printStacktrace();
    }
    return 0;
}
SESE_ENTER(Bundler::main)
