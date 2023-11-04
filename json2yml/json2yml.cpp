#include <sese/util/Initializer.h>
#include <sese/record/Marco.h>
#include <sese/io/File.h>
#include <sese/config/Json.h>
#include <sese/config/Yaml.h>

int main(int argc, char *argv[]) {
    sese::initCore(argc, argv);
    SESE_INFO("Initialization is complete");

    if (argc < 2) {
        SESE_ERROR("Missing necessary command-line arguments");
        SESE_INFO("Usage: %s <json file>", argv[0]);
        return -1;
    }

    std::string src = argv[1];
    auto srcFile = sese::io::File::create(src, BINARY_READ_EXISTED);
    if (srcFile == nullptr) {
        SESE_ERROR("File %s failed to open", src.c_str());
        return -1;
    }

    auto json = sese::Json::parse(srcFile.get(), 16);
    srcFile->close();
    if (json.isNull()) {
        SESE_ERROR("Parsing %s failed", src.c_str());
        return -1;
    }

    auto dest = src + ".yml";
    auto destFile = sese::io::File::create(dest, BINARY_WRITE_CREATE_TRUNC);
    if (destFile == nullptr) {
        SESE_ERROR("File %s failed to open", dest.c_str());
        return -1;
    }

    sese::Yaml::streamify(destFile.get(), json);
    destFile->close();
    SESE_INFO("Saved to %s", dest.c_str());

    return 0;
}