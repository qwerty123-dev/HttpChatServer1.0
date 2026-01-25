// src/chatserver/infrastructure/http/resources/user_resource.cpp
#include "chatserver/infrastructure/http/resources/user_resource.h"

#include "chatserver/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;
// Упрощаем доступ к JSON-библиотеке.

namespace chatserver::infrastructure::http::resources {

UserResource::UserResource(
    std::shared_ptr<chatserver::application::RegisterUserHandler> registerHandler,
    std::shared_ptr<chatserver::application::LoginUserHandler> loginHandler
) : registerHandler_(std::move(registerHandler)), loginHandler_(std::move(loginHandler)) {}
// Внедрение зависимостей: два обработчика application-слоя.
// Они инкапсулируют бизнес-логику регистрации и логина.

void UserResource::register_routes(chatserver::infrastructure::http::HttpRouter& router) {
    // Захватываем shared_ptr по значению, чтобы лямбда держала lifetime хендлеров
    auto regHandler = registerHandler_;
    // Регистрируем маршрут POST /register
    router.add_route("POST", "/register", [regHandler](const auto& req) {
        try {
            // Проверка: тело запроса не должно быть пустым.
            if (req.body.empty()) {
                json res{{"error", "empty body"}};
                return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
            }

            json j;
            try {
                // Парсим JSON из тела запроса.
                j = json::parse(req.body);
            } catch (const json::parse_error& e) {
                // Ошибка парсинга JSON — возвращаем 400.
                std::cerr << "[UserResource] /register json parse error: " << e.what()
                          << " body=[" << req.body << "]\n";
                json res{{"error", "invalid json"}};
                return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
            }

            // Проверяем наличие обязательных полей.
            if (!j.contains("username") || !j.contains("password") ||
                !j["username"].is_string() || !j["password"].is_string()) {
                json res{{"error", "invalid request: username and password required"}};
                return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
            }

            // Формируем команду application-слоя.
            chatserver::application::RegisterUserCommand cmd{
                j["username"].get<std::string>(),
                j["password"].get<std::string>()
            };

            // Вызываем бизнес-логику регистрации.
            std::int64_t userId = regHandler->handle(cmd);

            // Возвращаем ID созданного пользователя.
            json res{{"id", userId}};
            return chatserver::infrastructure::http::HttpResponse{200, res.dump()};
        }
        catch (const std::exception& ex) {
            // Ловим любые исключения — возвращаем 500.
            std::cerr << "[UserResource] /register exception: " << ex.what() << std::endl;
            json res{{"error", "internal server error"}};
            return chatserver::infrastructure::http::HttpResponse{500, res.dump()};
        }
        catch (...) {
            // Ловим неизвестные исключения.
            std::cerr << "[UserResource] /register unknown exception" << std::endl;
            json res{{"error", "internal server error"}};
            return chatserver::infrastructure::http::HttpResponse{500, res.dump()};
        }
    });

    // Аналогично — маршрут POST /login
    auto logHandler = loginHandler_;
    router.add_route("POST", "/login", [logHandler](const auto& req) {
        try {
            if (req.body.empty()) {
                json res{{"error", "empty body"}};
                return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
            }

            json j;
            try {
                j = json::parse(req.body);
            } catch (const json::parse_error& e) {
                std::cerr << "[UserResource] /login json parse error: " << e.what()
                          << " body=[" << req.body << "]\n";
                json res{{"error", "invalid json"}};
                return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
            }

            if (!j.contains("username") || !j.contains("password") ||
                !j["username"].is_string() || !j["password"].is_string()) {
                json res{{"error", "invalid request: username and password required"}};
                return chatserver::infrastructure::http::HttpResponse{400, res.dump()};
            }

            chatserver::application::LoginUserCommand cmd{
                j["username"].get<std::string>(),
                j["password"].get<std::string>()
            };

            std::int64_t userId = logHandler->handle(cmd);

            json res;
            if (userId != -1) res["id"] = userId;
            else res["error"] = "Invalid credentials";

            return chatserver::infrastructure::http::HttpResponse{200, res.dump()};
        }
        catch (const std::exception& ex) {
            std::cerr << "[UserResource] /login exception: " << ex.what() << std::endl;
            json res{{"error", "internal server error"}};
            return chatserver::infrastructure::http::HttpResponse{500, res.dump()};
        }
        catch (...) {
            std::cerr << "[UserResource] /login unknown exception" << std::endl;
            json res{{"error", "internal server error"}};
            return chatserver::infrastructure::http::HttpResponse{500, res.dump()};
        }
    });
}

} // namespace chatserver::infrastructure::http::resources

