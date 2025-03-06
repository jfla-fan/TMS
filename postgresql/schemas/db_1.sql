DROP SCHEMA IF EXISTS tms CASCADE;

CREATE SCHEMA IF NOT EXISTS tms;

-- Types

CREATE TYPE tms.user_role as ENUM ('user', 'admin');
CREATE TYPE tms.task_status as ENUM ('pending', 'in_progress', 'completed', 'expired');
CREATE TYPE tms.task_priority as ENUM ('low', 'normal', 'urgent');

CREATE TYPE tms.task AS (
	task_id 	INT,
	title		VARCHAR(255),
	description TEXT,
	category 	VARCHAR(255),
	deadline 	TIMESTAMPTZ,
	priority 	tms.task_priority,
	status 		tms.task_status,
	user_id 	INT,
	created_at 	TIMESTAMPTZ,
	updated_at 	TIMESTAMPTZ
);

-- Tables

CREATE TABLE IF NOT EXISTS tms.users (
    user_id             SERIAL          PRIMARY KEY,
    name                VARCHAR(255)    NOT NULL,
    last_name           VARCHAR(255)    NOT NULL,
    email               VARCHAR(255)    NOT NULL UNIQUE,
    login               VARCHAR(255)    NOT NULL UNIQUE,
    role                tms.user_role   NOT NULL,
    hashed_password     TEXT            NOT NULL,
    created_at          TIMESTAMPTZ     NOT NULL DEFAULT NOW(),
    updated_at          TIMESTAMPTZ     NOT NULL DEFAULT NOW(),
    
    CONSTRAINT email_unique UNIQUE (email),
    CONSTRAINT login_unique UNIQUE (login)
);

CREATE TABLE IF NOT EXISTS tms.tasks (
    task_id     SERIAL              PRIMARY KEY,
    title       VARCHAR(255)        NOT NULL,
    description TEXT,
    category    VARCHAR(255)        DEFAULT 'new',
    deadline    TIMESTAMPTZ,
    priority    tms.task_priority   NOT NULL DEFAULT 'normal',
    status      tms.task_status     NOT NULL DEFAULT 'pending',
    user_id     INT                 NOT NULL,
    created_at  TIMESTAMPTZ         NOT NULL DEFAULT NOW(),
    updated_at  TIMESTAMPTZ         NOT NULL DEFAULT NOW(),

    CONSTRAINT fk_user_id FOREIGN KEY (user_id)
        REFERENCES tms.users (user_id) ON DELETE CASCADE
)

-- Functions

CREATE OR REPLACE FUNCTION tms.get_tasks_by_filtering (
	p_user_id 			INT[] 				DEFAULT NULL,
	p_status 			tms.task_status[] 	DEFAULT NULL,
	p_priority 			tms.task_priority[] DEFAULT NULL,
	p_category 			TEXT[] 				DEFAULT NULL,
	p_deadline_start 	TIMESTAMPTZ 		DEFAULT NULL,
	p_deadline_end 		TIMESTAMPTZ 		DEFAULT NULL,
	p_page 				INT 				DEFAULT 1,
	p_limit 			INT 				DEFAULT 10
) RETURNS SETOF tms.task
AS $$
BEGIN
	RETURN QUERY
		SELECT *
			FROM tms.tasks
	  			WHERE (p_user_id IS NULL OR tms.tasks.user_id = ANY(p_user_id))
	    				AND (p_status IS NULL OR tms.tasks.status = ANY(p_status))
	    				AND (p_priority IS NULL OR tms.tasks.priority = ANY(p_priority))
	    				AND (p_category IS NULL OR tms.tasks.category = ANY(p_category))
	    				AND (p_deadline_start IS NULL OR tms.tasks.deadline >= p_deadline_start)
	    				AND (p_deadline_end IS NULL OR tms.tasks.deadline <= p_deadline_end)
  		ORDER BY tms.tasks.task_id
  		LIMIT p_limit
  		OFFSET (p_page - 1) * p_limit;
END;
$$ LANGUAGE PLPGSQL;