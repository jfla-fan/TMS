#pragma once

#include <userver/storages/query.hpp>


namespace tms::queries
{
    static const userver::storages::Query kFindUserById = "SELECT * FROM tms.users WHERE user_id = $1";
    static const userver::storages::Query kFindUserByLogin = "SELECT * FROM tms.users WHERE login = $1";

    static const userver::storages::Query kInsertUser = R"(
        INSERT INTO tms.users (
            name,
            last_name,
            email,
            login,
            role,
            hashed_password
        ) VALUES ($1, $2, $3, $4, $5, $6)
        RETURNING *
    )";

    static const userver::storages::Query kUpdateUserById = R"(
        UPDATE tms.users
        SET
            name            = COALESCE($1, name),
            last_name       = COALESCE($2, last_name),
            email           = COALESCE($3, email),
            login           = COALESCE($4, login),
            role            = COALESCE($5, role),
            hashed_password = COALESCE($6, hashed_password),
            updated_at      = NOW()
        WHERE user_id = $7
        RETURNING *;
    )";

    static const userver::storages::Query kDeleteUserById = R"(
        DELETE FROM tms.users
        WHERE user_id = $1
        RETURNING *
    )";

    static const userver::storages::Query kFindTaskById = "SELECT * FROM tms.tasks WHERE task_id = $1";
    static const userver::storages::Query kFindTaskByIdAndUser = "SELECT * FROM tms.tasks WHERE task_id = $1 AND user_id = $2";
    static const userver::storages::Query kFindTasksByFiltering = "SELECT * FROM tms.get_tasks_by_filtering($1, $2, $3, $4, $5, $6, $7, $8)";

    static const userver::storages::Query kInsertTask = R"(
        INSERT INTO tms.tasks (
            title,
            description,
            category,
            deadline,
            priority,
            status,
            user_id
        ) VALUES($1, $2, $3, $4, $5, $6, $7)
        RETURNING *
    )";

    static const userver::storages::Query kUpdateTaskById = R"(
        UPDATE tms.tasks
        SET
            title       = COALESCE($1, title),
            description = COALESCE($2, description),
            category    = COALESCE($3, category),
            deadline    = COALESCE($4, deadline),
            priority    = COALESCE($5, priority),
            status      = COALESCE($6, status),
            user_id     = COALESCE($7, user_id),
            updated_at  = NOW()
        WHERE task_id = $8
        RETURNING *
    )";

    static const userver::storages::Query kUpdateTaskByIdAndUser = R"(
        UPDATE tms.tasks
        SET
            title       = COALESCE($1, title),
            description = COALESCE($2, description),
            category    = COALESCE($3, category),
            deadline    = COALESCE($4, deadline),
            priority    = COALESCE($5, priority),
            status      = COALESCE($6, status),
            updated_at  = NOW()
        WHERE task_id = $7 AND user_id = $8
        RETURNING *
    )";

    static const userver::storages::Query kDeleteTaskById = R"(
        DELETE FROM tms.tasks
        WHERE task_id = $1
        RETURNING *
    )";

    static const userver::storages::Query kDeleteTaskByIdAndUser = R"(
        DELETE FROM tms.tasks
        WHERE task_id = $1 AND user_id = $2
        RETURNING *
    )";
}