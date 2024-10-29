// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "DnsServer.h"

#include <sese/internal/service/dns/DnsService.h>

using sese::internal::net::service::dns::DnsService;
using sese::service::dns::DnsServer;

#define COV auto service = dynamic_cast<DnsService *>(this->service.get())

DnsServer::DnsServer() {
    service = std::make_unique<DnsService>();
}

bool DnsServer::bind(const net::IPAddress::Ptr &address) {
    COV;
    return service->bind(address);
}

void DnsServer::setCallback(const Callback &callback) {
    COV;
    service->setCallback(callback);
}

bool DnsServer::addUpstreamNameServer(const std::string &ip, uint16_t port) {
    COV;
    return service->addUpstreamNameServer(ip, port);
}

void DnsServer::addUpstreamNameServer(const net::IPAddress::Ptr &address) {
    COV;
    service->addUpstreamNameServer(address);
}

void DnsServer::addRecord(const std::string &name, const net::IPAddress::Ptr &address) {
    COV;
    service->addRecord(name, address);
}

bool DnsServer::startup() {
    return service->startup();
}

bool DnsServer::shutdown() {
    return service->shutdown();
}

int DnsServer::getLastError() {
    return service->getLastError();
}

std::string DnsServer::getLastErrorMessage() {
    return service->getLastErrorMessage();
}
