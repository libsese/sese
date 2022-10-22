#include <sese/config/xml/XmlUtil.h>
#include <sese/BufferedStream.h>
#include <sese/StringBuilder.h>
#include <sese/Util.h>

namespace sese::xml {

    bool XmlUtil::tokenizer(const Stream::Ptr &inputStream, sese::xml::XmlUtil::Tokens &tokens) noexcept {
        BufferedStream bufferedStream(inputStream);
        StringBuilder stringBuilder;
        char ch;
        int64_t len;
        while ((len = bufferedStream.read(&ch, 1 * sizeof(char))) != 0) {
            if (sese::isSpace(ch)) {
                if (stringBuilder.empty()) continue;
                tokens.push(stringBuilder.toString());
                stringBuilder.clear();
            } else if (ch == '>') {
                if (!stringBuilder.empty())
                    tokens.push(stringBuilder.toString());
                tokens.push({ch});
                stringBuilder.clear();
            } else if (ch == '<') {
                if (!stringBuilder.empty()) {
                    tokens.push(stringBuilder.toString());
                    stringBuilder.clear();
                }
                tokens.push({ch});

            } else if (ch == '/') {
                if (tokens.front() == "<") {
                    if (!stringBuilder.empty())
                        tokens.push(stringBuilder.toString());
                    stringBuilder.clear();
                    tokens.push({ch});
                } else {
                    if (!stringBuilder.empty())
                        tokens.push(stringBuilder.toString());
                    tokens.push({ch});
                    stringBuilder.clear();
                }
            } else {
                stringBuilder.append(ch);
            }
        }
        return true;
    }
}// namespace sese::xml