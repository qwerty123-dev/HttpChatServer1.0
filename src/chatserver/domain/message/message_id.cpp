#include "chatserver/domain/message/message_id.h"

namespace chatserver::domain {
// Пространство имен domain - слой предметной области (DDD).
MessageId::MessageId(std::int64_t value)
    : value_(value) {}

// Конструктор принимает 64-битное число и сохраняет его во внутреннем поле value_.
// Используем список инициализации — это самый эффективный способ задать значение.

std::int64_t MessageId::value() const {
    return value_;
}
// Геттер, возвращающий числовое значение идентификатора сообщения
// Возвращается по значению, так как это примитивный тип.

// MessageId MessageId::generate() {
//         static std::random_device rd;
//         // random_device — источник энтропии, обычно использующий системный RNG.
//         // static гарантирует, что объект создаётся один раз за всё время работы программы.
//         static std::mt19937_64 gen(rd());
//         // mt19937_64 — высококачественный 64-битный генератор псевдослучайных чисел.
//         // Инициализируется значением из random_device.
//         // static делает генератор единым для всех вызовов generate().
//         static std::uniform_int_distribution<std::int64_t> dis(1, INT64_MAX);
//         // Равномерное распределение по диапазону [1, INT64_MAX].
//         // Это обеспечивает огромный диапазон возможных ID и минимальный риск коллизий.
//         return MessageId(dis(gen));
//         // Генерируем случайное число и создаём новый MessageId.
//         // Возвращаем его по значению — это Value Object, копирование дешёвое.
//     }


bool MessageId::operator==(const MessageId& other) const {
    return value_ == other.value_;
}
// Оператор сравнения на равенство.
// Два MessageId равны, если совпадают их числовые значения.

}

