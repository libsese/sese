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

std::shared_ptr<sese::UniReader> sese::UniReader::create(const std::string &fileName) noexcept {
    auto fileStream = FileStream::create(fileName, TEXT_READ_EXISTED);
    if (!fileStream) {
        return nullptr;
    }
    auto reader = std::make_shared<StreamReader>(fileStream);
    auto uniReader = new UniReader();
    uniReader->fileStream = fileStream;
    uniReader->reader = reader;
    return std::shared_ptr<sese::UniReader>(uniReader);
}