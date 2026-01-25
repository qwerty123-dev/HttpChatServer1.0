1. login_user_handler.cpp добавляем исключения
2. openssl_message_encryptor.h заменить стринг на 
secure buffer (std::vector<uint8_t> + очистка)
3. openssl_message_encryptor.cpp - много криптографических ошибок
4. openssl_password_encryptor.h заменить стринг на вектор и 
добавить количество итераций (минимум 300k–600k)
5. postgress_message_repository.h/.cpp переписать в идеальный продакшен-вариант
6. postgress_user_repository.h/.cpp переписать в идеальный продакшен-вариант
7. Синхронный многопоточный сервер

Чистая пересборка:
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel

Тесты:
ctest --output-on-failure
