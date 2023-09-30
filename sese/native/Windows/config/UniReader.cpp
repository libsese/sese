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

std::shared_ptr<sese::UniReader> sese::UniReader::create(const std::string &fileName) noexcept {
    auto fileStream = io::FileStream::create(fileName, TEXT_READ_EXISTED_W);
    if (!fileStream) {
        return nullptr;
    }
    auto reader = std::make_shared<io::WStreamReader>(fileStream);
    auto uniReader = new UniReader();
    uniReader->fileStream = fileStream;
    uniReader->reader = reader;
    return std::shared_ptr<sese::UniReader>(uniReader);
}
