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
    return sese::EncodingConverter::toString(str);
}

bool sese::UniReader::open(const std::string &fileName) noexcept {
    fileStream = FileStream::create(fileName, TEXT_READ_EXISTED_W);
    if(!fileStream) return false;
    reader = std::make_unique<WStreamReader>(fileStream);
    return true;
}
