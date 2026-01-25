#pragma once

#include "chatserver/domain/message/message_id.h"
// Подключаем Value Object MessageId - уникальный идентификатор сообщения.
#include "chatserver/domain/message/message_text.h"
// Подключаем Value Object MessageText - текст сообщения.
#include "chatserver/domain/user/user_id.h"
// Подключаем Value Object UserId - идентификатор отправителя.
#include "chatserver/domain/common/timestamp.h"
// Подключаем Value Object Timestamp - время создания сообщения.

namespace chatserver::domain::message {
// Пространство имен messgae внутри domain - логическая группировка
// всех сущностей, связанных с сообщениями.

class Message {
// Доменная сущность (Entity), представляющая сообщение в системе.
// В отличие от Value Object, сущность имеет устойчивую идентичность (id_).
public:
    Message(
        MessageId id,
        UserId senderId,
        MessageText text,
        Timestamp createdAt
    );

    Message(UserId senderId, MessageText text, Timestamp createdAt);
    // Основной конструктор сущности Message.
    // Используется при создании нового сообщения или загрузке из БД.
    // Все параметры передаются по значению, затем перемещаются в поля —
    // это упрощает вызов и позволяет использовать move semantics.

    const MessageId& id() const;
    // Геттер, возвращающий ссылку на идентификатор сообщения.
    const UserId& sender_id() const;
    // Геттер, возвращающий идентификатор отправителя.
    const MessageText& text() const;
    // Геттер, возвращающий текст сообщения.
    const Timestamp& created_at() const;
    // Геттер, возвращающий время создания сообщения.

private:
    MessageId id_;
    // Уникальный идентификатор сообщения — определяет сущность.
    UserId senderId_;
    // Идентификатор пользователя, который отправил сообщение.
    MessageText text_;
    // Текст сообщения — Value Object, всегда валидный.
    Timestamp createdAt_;
    // Время создания сообщения — Value Object, неизменяемый.
};

}

