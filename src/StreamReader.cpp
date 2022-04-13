#include "StreamReader.h"
#include "IOException.h"
#include "Util.h"
#include <sstream>

namespace sese {

    StreamReader::StreamReader(Stream::Ptr source) {
        this->sourceStream = std::move(source);
        this->bufferStream = std::make_shared<ByteBuilder>();
    }

    bool StreamReader::read(char &ch) {
        if(bufferStream->read(&ch, 1) == 0){
            /// 未命中缓存，尝试从源中读取
            auto len = preRead();
            if(0 == len){
                /// FileStream 读至文件尾返回值为 0
                /// 源已被读完
                return false;
            }else if (len < 0) {
                /// FileStream、Socket 等出错返回 -1
                /// 源出错
                throw IOException(getErrorString());
            }
        }
        /// 嘿嘿，读取到一个字符
        return true;
    }

    std::string StreamReader::readLine() {
        std::stringstream string;
        auto canReadSize = bufferStream->getLength() - bufferStream->getCurrentReadPos();
        if(canReadSize == 0){
            canReadSize += preRead();
        }

        if(canReadSize > 0){
            /// 有东西可读
            char ch;
            bool isFirst = true;
            while(read(ch)) {
                if(ch == '\r' || ch == '\n'){
                    if(isFirst) continue;
                    break;
                }
                string << ch;
                isFirst = false;
            }
        }
        return string.str();
    }

    int64_t StreamReader::preRead() {
        char buffer[STREAM_BYTESTREAM_SIZE_FACTOR];
        auto len = sourceStream->read(buffer, STRING_BUFFER_SIZE_FACTOR);
        if (0 < len) {
            bufferStream->write(buffer, len);
        }
        return len;
    }

}// namespace sese