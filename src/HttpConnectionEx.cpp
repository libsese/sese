#include "HttpConnectionEx.h"

HttpConnectionEx::HttpConnectionEx(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &io_context)
    : timer(io_context),
      service(service) {
}
