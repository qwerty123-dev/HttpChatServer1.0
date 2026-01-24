// src/main.cpp
#include "chatserver/bootstrap/bootstrap.h"
#include "chatserver/common/common.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <memory>

int main() {
    // --- Жёстко заданные параметры ---
    const std::string db_host   = "localhost";
    const std::string db_port   = "5432";
    const std::string db_name   = "chat";
    const std::string db_user   = "chat";
    const std::string db_pass   = "password123";

    const std::string secret    = "supersecretkey";
    const std::string address   = "0.0.0.0";
    const int         serverPort = 8080;

    // Формируем строку подключения к Postgres
    std::string dbConnStr =
        "host=" + db_host +
        " port=" + db_port +
        " dbname=" + db_name +
        " user=" + db_user +
        " password=" + db_pass;

    // Маскированная версия для логов
    std::string dbConnStrNoPass =
        "host=" + db_host +
        " port=" + db_port +
        " dbname=" + db_name +
        " user=" + db_user +
        " password=***";

    std::cerr << "[INFO] DB connection string: [" << dbConnStrNoPass << "]" << std::endl;

    try {
        auto ctx = chatserver::bootstrap::initialize_app(
            dbConnStr,
            secret,
            address,
            serverPort
        );

        std::cout << "ChatServer REST API started on " << address << ":" << serverPort << std::endl;
        ctx.server->run();
    }
    catch (const std::exception& ex) {
        std::cerr << "[FATAL] Initialization error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

