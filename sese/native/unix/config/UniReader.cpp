#include "sese/config/UniReader.h"
#include "sese/convert/EncodingConverter.h"

//sese::UniReader::UniReader(const std::string &fileName) {
//    fileStream = std::make_shared<FileStream>(fileName, TEXT_READ_EXISTED);
//    if(!fileStream->good()) throw sese::IOException(sese::getErrorString());
//    reader = std::make_shared<StreamReader>(fileStream);
//}

sese::UniReader::~UniReader() {
    fileStream->close();
}

std::string sese::UniReader::readLine() {
    return reader->readLine();
}

std::shared_ptr<sese::UniReader> sese::UniReader::create(const std::string &file_name) noexcept {
    auto file_stream = sese::io::FileStream::create(file_name, TEXT_READ_EXISTED);
    if (!file_stream) {
        return nullptr;
    }
    auto reader = std::make_shared<sese::io::StreamReader>(file_stream);
    auto uni_reader = new UniReader();
    uni_reader->fileStream = file_stream;
    uni_reader->reader = reader;
    return std::shared_ptr<sese::UniReader>(uni_reader);
}