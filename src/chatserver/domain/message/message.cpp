#include "chatserver/domain/message/message.h"

namespace chatserver::domain::message {
// Пространство имён message внутри domain — логическая область,
// содержащая сущности, связанные с сообщениями.

Message::Message(
    MessageId id,
    UserId senderId,
    MessageText text,
    Timestamp createdAt
)
    : id_(id)
    , senderId_(senderId)
    , text_(std::move(text))
    , createdAt_(createdAt) {}
// Основной конструктор сущности Message.
// Все параметры передаются по значению, затем:
//   • id_ и senderId_ копируются (оба лёгкие Value Object'ы)
//   • text_ перемещается (строка внутри может быть большой)
//   • createdAt_ копируется (int64_t внутри, копирование дешёвое)
// Такой подход упрощает вызов конструктора и использует move semantics там,
// где это даёт реальную выгоду.
Message::Message(
    UserId senderId,
    MessageText text,
    Timestamp createdAt
)
    : id_(0)
    , senderId_(senderId)
    , text_(std::move(text))
    , createdAt_(createdAt) {}

const MessageId& Message::id() const {
    return id_;
}
// Геттер, возвращающий идентификатор сообщения.
// Возвращаем const&, чтобы избежать копирования и защитить от изменений.

const UserId& Message::sender_id() const {
    return senderId_;
}
// Геттер, возвращающий идентификатор отправителя сообщения.

const MessageText& Message::text() const {
    return text_;
}
// Геттер, возвращающий текст сообщения.
// Возвращение const& исключает копирование строки.
const Timestamp& Message::created_at() const {
    return createdAt_;
}
// Геттер, возвращающий время создания сообщения.
}

