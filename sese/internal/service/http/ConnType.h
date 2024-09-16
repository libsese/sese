#pragma once

namespace sese::internal::service::http {

enum class ConnType {
    FILTER,
    FILE_DOWNLOAD,
    CONTROLLER,
    NONE
};

}
