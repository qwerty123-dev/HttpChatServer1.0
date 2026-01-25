#include "chatserver/application/handlers/send_message_handler.h"
// Подключаем заголовок с объявлением SendMessageHandler — обработчика use‑case
// "Send Message" в application‑слое.

#include <iostream>
#include <exception>
#include <stdexcept>

namespace chatserver::application {

SendMessageHandler::SendMessageHandler(
    std::shared_ptr<domain::services::MessageEncryptor> encryptor,
    // MessageEncryptor — доменный сервис, отвечающий за шифрование текста.
    // Handler не знает алгоритм (AES/ChaCha20/etc.) — это скрыто за интерфейсом.
    std::shared_ptr<infrastructure::repository::MessageRepository> messageRepository
    // MessageRepository — инфраструктурный компонент, который знает о БД.
    // Handler работает только с интерфейсом, не зная SQL.
)
    : encryptor_(std::move(encryptor))
    // Сохраняем сервис шифрования. std::move — корректно для shared_ptr.
    , messageRepository_(std::move(messageRepository)) {}
    // Сохраняем репозиторий сообщений. Handler полностью готов выполнять use‑case.

std::int64_t SendMessageHandler::handle(const SendMessageCommand& command) {
    // Основной метод use‑case "отправить сообщение".
    // Принимает SendMessageCommand (sender_id, text).
    // Преобразует данные в доменные объекты, шифрует текст, создаёт Message и 
    // сохраняет его через репозиторий.

    try {
        if (!encryptor_) {
            // Проверяем корректность внедрения зависимостей.
            // Это защита от ошибок конфигурации.
            std::cerr << "[SendMessageHandler] ERROR: encryptor_ is null" << std::endl;
            throw std::runtime_error("encryptor not initialized");
        }
        if (!messageRepository_) {
            // Аналогичная проверка для репозитория.
            std::cerr << "[SendMessageHandler] ERROR: messageRepository_ is null" << std::endl;
            throw std::runtime_error("messageRepository not initialized");
        }

        std::string encrypted;
        try {
            encrypted = encryptor_->encrypt(command.text);
            // 1. Шифруем текст сообщения через доменный сервис.
            // Handler не знает, как именно происходит шифрование.
        } catch (const std::exception& e) {
            // Логируем ошибку шифрования и пробрасываем дальше.
            std::cerr << "[SendMessageHandler] encryption failed: " << e.what() << std::endl;
            throw;
        }

        // Создаём доменную сущность Message.
        domain::message::Message message(
            //domain::MessageId::generate(),  // Генерируем уникальный ID сообщения.
            domain::UserId(command.sender_id), // Превращаем sender_id в доменный UserId.
            domain::MessageText(encrypted), // Оборачиваем зашифрованный текст в Value Object.
            domain::Timestamp::now()  // Фиксируем время создания.
        );

        try {
            // 2. Сохраняем сообщение через репозиторий.
            // Handler не знает SQL — только вызывает интерфейс.
            return messageRepository_->save(message);
        } catch (const std::exception& e) {
            // Логируем ошибку сохранения и пробрасываем дальше.
            std::cerr << "[SendMessageHandler] messageRepository save failed: " << e.what() << std::endl;
            throw;
        }
    } catch (const std::exception& ex) {
        // Ловим любые std::exception — логируем и пробрасываем дальше.
        // Это позволяет HTTP‑слою вернуть корректный статус (например, 500).
        std::cerr << "[SendMessageHandler] EXCEPTION: " << ex.what() << std::endl;
        throw;
    } catch (...) {
        // Ловим любые другие исключения (редко, но возможно).
        std::cerr << "[SendMessageHandler] UNKNOWN EXCEPTION" << std::endl;
        throw;
    }
}

} // namespace chatserver::application

