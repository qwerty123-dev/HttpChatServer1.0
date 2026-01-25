// src/chatserver/infrastructure/http/resources/message_resource.cpp
#include "chatserver/infrastructure/http/resources/message_resource.h"
#include "chatserver/infrastructure/http/http_response.h"

#include "chatserver/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;
// Упрощаем доступ к JSON-библиотеке.

namespace chatserver::infrastructure::http::resources {

MessageResource::MessageResource(std::shared_ptr<chatserver::application::SendMessageHandler> sendHandler)
    : sendHandler_(std::move(sendHandler)) {}
// Внедрение зависимости: обработчик use case "отправить сообщение".
// Хранится в shared_ptr, чтобы ресурс мог безопасно использовать его в лямбдах.

void MessageResource::register_routes(chatserver::infrastructure::http::HttpRouter& router) {
    // Захватываем shared_ptr по значению, чтобы лямбда держала lifetime хендлера
    auto handler = sendHandler_;
    router.add_route("POST", "/send_message", [handler](const auto& req) {
        if (req.body.empty()) {
            json res{{"error", "empty body"}};
            return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
        }

        json j;
        try {
            j = json::parse(req.body);
        } catch (const json::parse_error& e) {
            std::cerr << "[MessageResource] json parse error: " << e.what()
                      << " body=[" << req.body << "]\n";
            json res{{"error", "invalid json"}};
            return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
        }

        // Проверяем поля и типы (без receiver_id — согласно схеме БД)
        if (!j.contains("sender_id") || !j.contains("text") ||
            j["sender_id"].is_null() || !j["text"].is_string() ||
            !j["sender_id"].is_number_integer()) {
            json res{{"error", "invalid request: sender_id (int) and text (string) required"}};
            return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
        }

        chatserver::application::SendMessageCommand cmd{
            j["sender_id"].get<std::int64_t>(),
            j["text"].get<std::string>()
        };

        try {
            std::int64_t messageId = handler->handle(cmd);
            json res{{"id", messageId}};
            return chatserver::infrastructure::http::HttpResponse{200, res.dump()};
        } catch (const std::exception& ex) {
            std::cerr << "[MessageResource] /send_message exception: " << ex.what() << std::endl;
            json res{{"error", "internal server error"}};
            return chatserver::infrastructure::http::HttpResponse{500, res.dump()};
        } catch (...) {
            std::cerr << "[MessageResource] /send_message unknown exception" << std::endl;
            json res{{"error", "internal server error"}};
            return chatserver::infrastructure::http::HttpResponse{500, res.dump()};
        }
    });
}

} // namespace chatserver::infrastructure::http::resources

