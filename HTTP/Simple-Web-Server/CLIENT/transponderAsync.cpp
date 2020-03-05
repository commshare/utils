//
// Created by jianbo on 7/27/18.
//
#include <utility>
#include "transponder/transponder.h"

namespace XService {
bool Transponder::Init(std::shared_ptr<boost::asio::io_service> service,
                       const std::string &server) {
  if (NULL == service || server.empty()) {
    return false;
  }
  client_ = std::make_shared<HttpClient>(server);
  if (client_) {
    client_->io_service = service;
    return true;
  }
  return false;
}

// async
void Transponder::Request(
    const std::string &method, const std::string &path,
    const SimpleWeb::CaseInsensitiveMultimap &headers, std::istream &content,
    std::function<void(std::shared_ptr<HttpClient::Response>,
                       const SimpleWeb::error_code &)> request_callback) {
  client_->request(method, path, content, headers, std::move(request_callback));
}

// sync
std::shared_ptr<HttpClient::Response> Transponder::Request(
    const std::string &method, const std::string &path,
    const SimpleWeb::CaseInsensitiveMultimap &headers, std::istream &content) {
  return client_->request(method, path, content, headers);
}
}