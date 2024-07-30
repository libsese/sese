#include "sese/config/UniReader.h"
#include "sese/convert/EncodingConverter.h"

//sese::UniReader::UniReader(const std::string &fileName) {
//    fileStream = std::make_shared<FileStream>(fileName, TEXT_READ_EXISTED_W);
//    if(!fileStream->good()) throw sese::IOException(sese::getErrorString());
//    reader = std::make_shared<WStreamReader>(fileStream);
//}

sese::UniReader::~UniReader() {
    fileStream->close();
}

std::string sese::UniReader::readLine() {
    std::wstring str = reader->readLine();
    // return sese::EncodingConverter::toString(str);
    return convert.to_bytes(str);
}

std::shared_ptr<sese::UniReader> sese::UniReader::create(const std::string &file_name) noexcept {
    auto file_stream = io::FileStream::create(file_name, io::File::T_READ_W);
    if (!file_stream) {
        return nullptr;
    }
    auto reader = std::make_shared<io::WStreamReader>(file_stream);
    auto uni_reader = MAKE_SHARED_PRIVATE(UniReader);
    uni_reader->fileStream = file_stream;
    uni_reader->reader = reader;
    return uni_reader;
}
