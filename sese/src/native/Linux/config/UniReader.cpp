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

bool sese::UniReader::open(const std::string &fileName) noexcept {
    fileStream = FileStream::create(fileName, TEXT_READ_EXISTED);
    if(!fileStream) return false;
    reader = std::make_unique<StreamReader>(fileStream);
    return true;
}