#!/bin/bash
# migrate_db.sh
# Скрипт для создания схемы БД PostgreSQL для ChatServer

DB_HOST="${DB_HOST:-localhost}"
DB_PORT="${DB_PORT:-5432}"
DB_NAME="${DB_NAME:-chat}"
DB_USER="${DB_USER:-chat}"
DB_PASSWORD="${DB_PASSWORD:-password123}"

export PGPASSWORD="$DB_PASSWORD"

echo "Подключаемся к $DB_NAME на $DB_HOST:$DB_PORT как $DB_USER"

# Создаём базу, если не существует
psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -tc \
"SELECT 1 FROM pg_database WHERE datname = '$DB_NAME'" | grep -q 1 || \
psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -c "CREATE DATABASE $DB_NAME;"

# Создаём таблицы
psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" <<'EOF'
-- Таблица пользователей
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(255) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Таблица сообщений
CREATE TABLE IF NOT EXISTS messages (
    id SERIAL PRIMARY KEY,  -- AUTO INCREMENT для идентификатора
    sender_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    text TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
EOF

echo "Миграция завершена."

