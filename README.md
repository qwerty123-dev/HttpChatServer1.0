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
9. Синхронный многопоточный сервер

Чистая пересборка:
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel

Тесты:
ctest --output-on-failure
