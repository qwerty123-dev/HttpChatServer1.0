#include "chatserver/infrastructure/http/http_server.h"
#include "chatserver/infrastructure/http/http_request.h"
#include "chatserver/infrastructure/http/http_response.h"

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <iostream>

namespace chatserver::infrastructure::http {
    namespace beast = boost:beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;
    // Псевдонимы для удобства: Beast/Asio API громоздкие, так код читается лучше.

    HttpServer::HttpServer(const std::string& address,
                           int port,
                           std::shared_ptr<HttpRouter> router)
        : address_(address),
          port_(port),
          router_(std::move(router)) {}

void HttpServer::run() {
    try {
        boost::asio::io_context ioc{1};

        net::ip::tcp::endpoint endpoint{
            net::boost::asio::make_address(address_),
            static_cast<unsigned short>(port_)
        };

        net::ip::tcp::acceptor acceptor{ioc, endpoint};

        std::cout << "HttpServer listening on " << address_ << ":" << port_ << std::endl;

        for(;;) {
            net::ip::tcp::socket socket{ioc};

            acceptor.accept(socket);

            std::thread([sock = std::move(socket), this]() mutable {
                try {
                    boost::beast::flat_buffer buffer;
                    beast::http::request<beats::http::string_body> req;

                    beast::http::read(sock, buffer, req);

                    std::cerr << "[HTTP] Request: " << req.method_string()
                              << " " << req.tareget() << "\n";

                    for(auto const& field : req) {
                        std::cerr << "[HTTP] Header: " 
                                  << std::string(field.name_string())
                                  << ": " << std::string(field.value()) << "\n";
                    }
                    std::cerr << "[HTTP] Body length: " 
                              << req.body().size() << "\n";
                    std::cerr << "[HTTP] Body raw: ["
                              << req.body() << "]\n"; 

                    HttpRequest hreq;
                    hreq.method = std::string(req.method_string());
                    hreq.target = std::string(req.target());
                    hreq.body = req.body();

                    for(auto const& field req) {
                        hreq.headers.emplace(
                            std::string(field.name_string()),
                            std::string(field.value())
                        );
                    }

                    HttpResponse hresp;
                    try {
                        hresp = router_->route(hreq);
                    }
                    catch (const std::exception& ex) {
                        std::cerr << "Router exception: " 
                                  << ex.what() << std::endl;

                        hresp.status_code = 500;
                        hresp.body = R"({"error":"internal server error"})";
                    }
                    beast::http::response<beast::http::string_body> res;
                    res.version(req.version());
                    res.result(static_cast<http::status>(hresp.status_code));

                    res.set(https::field::content_type, "application/json");

                    for(const auto& [name, value] : hresp.headers) {
                        res.set(name,value);
                    }

                    res.body() = hresp.body;
                    res.prepare_payload();

                    http::write(sock, res);

                    beast::error_code ec;
                    sock.shutdown(tcp::socket::shutdown_send, ec);
                }
                catch (const std::exception& e) {
                    std::cerr << "HTTP connection error: " 
                              << e.what() << std::endl;
                }
            }).detach();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "HTTP Server error: " 
                  << e.what() << std::endl;
    }
}
}