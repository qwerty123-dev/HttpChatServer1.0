1. Убираем generate() из message.h
2. login_user_handler.cpp добавляем исключения
3. openssl_message_encryptor.h заменить стринг на 
secure buffer (std::vector<uint8_t> + очистка)
4. openssl_message_encryptor.cpp - много криптографических ошибок
5. openssl_password_encryptor.h заменить стринг на вектор и 
добавить количество итераций (минимум 300k–600k)
6. переписать репозитории (асинхронность + проверка транзакций)
7. postgress_message_repository.h/.cpp переписать в идеальный продакшен-вариант
8. postgress_user_repository.h/.cpp переписать в идеальный продакшен-вариант
9. Синхронный многопоточный сервер или асинхронный пул
10. 

Необходимые библиотеки для продакшен-ready HTTP-сервера на C++:
1. Сетевой стек (основа)
- Boost.Asio (асинхронный I/O, TCP, таймеры, executors, корутины)
- Boost.Beast (HTTP/1.1, WebSocket, буферы, парсеры, сериализаторы)
2. SSL/TLS (HTTPS)
- OpenSSL (boost::asio::ssl::stream)
3. JSON / сериализация (boost::json) 
4. Пул потоков/планировщик
- собственный пул (std::thread + work_guard) 
5. Тестирование 
- Юнит-тесты (GoogleTest)
- HTTP-тестирование (Boost.Beast test helpers, cURL)
6. Логирование (Boost.Log)
7. Метрики/мониторинг
- prometheus-cpp
- OpenTelemetry C++
8. Безопасность
Для защиты от DoS, rate-limit, throttling:
- собственные middleware
Для JWT/OAuth:
- Boost.Beast + custom crypto
9. Работа с БД (libpqxx)
10. Утилиты и инфраструктура
Конфигурация:
- Boost.Program_options
Форматирование:
- fmt (используется внутри spdlog)
Контейнеры:
- Boost.Container

# чистая пересборка
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel

# запустить все тесты
ctest --output-on-failure
# или запустить конкретный тест
./password_hasher_test

