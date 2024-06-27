#define SESE_C_LIKE_FORMAT

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
    auto src_file = sese::io::File::create(src, BINARY_READ_EXISTED);
    if (src_file == nullptr) {
        SESE_ERROR("File %s failed to open", src.c_str());
        return -1;
    }

    auto json = sese::Json::parse(src_file.get(), 16);
    src_file->close();
    if (json.isNull()) {
        SESE_ERROR("Parsing %s failed", src.c_str());
        return -1;
    }

    auto dest = src + ".yml";
    auto dest_file = sese::io::File::create(dest, BINARY_WRITE_CREATE_TRUNC);
    if (dest_file == nullptr) {
        SESE_ERROR("File %s failed to open", dest.c_str());
        return -1;
    }

    sese::Yaml::streamify(dest_file.get(), json);
    dest_file->close();
    SESE_INFO("Saved to %s", dest.c_str());

    return 0;
}