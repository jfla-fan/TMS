# TMS
Task Management System

Service that manages tasks, its deadlines, sends notifications. \
Look for openapi.yml to check out the backend api.

Dev Containers vs code extension may be used for comfortable development.

## Internal useful commands

Some commands that you may find useful during development:

* `su -s /bin/bash user` - change user to "user".
* `chown -R user:users build_debug` - change ownership of build directory if accidently tried to build with "root"
* `curl -X POST "http://localhost:8080/v1/auth/register" \
    -H "Content-Type: application/json" \
    -d '{
    "name": "John",
    "last_name": "Doe",
    "email": "john.doe@example.com",
    "login": "john_doe",
    "password": "securepassword123"
}'` - create a user;

    Expected returned token:
    `eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6ImpvaG4uZG9lQGV4YW1wbGUuY29tIiwiZXhwIjoxNzM0OTU0Njc5LCJpYXQiOjE3MzQ5NTM3NzksImlzcyI6InRtcy5zZXJ2aWNlIiwibG9naW4iOiJqb2huX2RvZSIsInN1YiI6IjMiLCJ1c2VyX3JvbGUiOiJ1c2VyIn0.Yeqkz74hOI3K5woIron-LSVRamrHV6BpIDEcECTu_L4`

* `curl -X POST "http://localhost:8080/v1/auth/login" \
    -H "Content-Type: application/json" \
    -d '{
    "login": "admin_login",
    "password": "Admin@123"
}'` - log in, returns token;

* `curl -X GET "http://localhost:8080/v1/tasks/1" \
    -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6ImpvaG4uZG9lQGV4YW1wbGUuY29tIiwiZXhwIjoxNzM4Njk2ODE0LCJpYXQiOjE3Mzg2NzUyMTQsImlzcyI6InRtcy5zZXJ2aWNlIiwibG9naW4iOiJqb2huX2RvZSIsInN1YiI6IjMiLCJ1c2VyX3JvbGUiOiJ1c2VyIn0.8XSgajauiBpBrn4GBc7iuFZB9iq7W3CxwK9iGZHPmWo"` - get a task

    Expected returned:
    `{
        "task_id": 1,
        "title": "Sample Task",
        "description": "This is a sample task description.",
        "category": "Work",
        "deadline": "2023-12-31T23:59:00Z",
        "priority": "normal",
        "status": "pending",
        "user_id": 3,
        "created_at": "2023-10-01T10:00:00Z",
        "updated_at": "2023-10-01T15:30:00Z"
    }`

* `curl -X POST "http://localhost:8080/v1/tasks" \
    -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6ImpvaG4uZG9lQGV4YW1wbGUuY29tIiwiZXhwIjoxNzM4Njk2ODE0LCJpYXQiOjE3Mzg2NzUyMTQsImlzcyI6InRtcy5zZXJ2aWNlIiwibG9naW4iOiJqb2huX2RvZSIsInN1YiI6IjMiLCJ1c2VyX3JvbGUiOiJ1c2VyIn0.8XSgajauiBpBrn4GBc7iuFZB9iq7W3CxwK9iGZHPmWo" \
    -H "Content-Type: application/json" \
    -d '{
    "title": "Implement user authentication",
    "description": "Develop and test JWT-based authentication",
    "category": "work",
    "deadline": "2025-01-23T11:58:17Z",
    "priority": "urgent",
    "status": "pending"
    }'` - create a task.

* ` curl -X DELETE "http://localhost:8080/v1/tasks/2" \
    -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6ImpvaG4uZG9lQGV4YW1wbGUuY29tIiwiZXhwIjoxNzM4Njk2ODE0LCJpYXQiOjE3Mzg2NzUyMTQsImlzcyI6InRtcy5zZXJ2aWNlIiwibG9naW4iOiJqb2huX2RvZSIsInN1YiI6IjMiLCJ1c2VyX3JvbGUiOiJ1c2VyIn0.8XSgajauiBpBrn4GBc7iuFZB9iq7W3CxwK9iGZHPmWo" ` - delete task.

* `psql postgresql://user:password@localhost:5432/pg_tms_db` - postgresql connection command.


## Makefile

Makefile contains typicaly useful targets for development:

* `make build-debug` - debug build of the service with all the assertions and sanitizers enabled
* `make build-release` - release build of the service with LTO
* `make test-debug` - does a `make build-debug` and runs all the tests on the result
* `make test-release` - does a `make build-release` and runs all the tests on the result
* `make service-start-debug` - builds the service in debug mode and starts it
* `make service-start-release` - builds the service in release mode and starts it
* `make` or `make all` - builds and runs all the tests in release and debug modes
* `make format` - autoformat all the C++ and Python sources
* `make clean-` - cleans the object files
* `make dist-clean` - clean all, including the CMake cached configurations
* `make install` - does a `make build-release` and runs install in directory set in environment `PREFIX`
* `make install-debug` - does a `make build-debug` and runs install in directory set in environment `PREFIX`
* `make docker-COMMAND` - run `make COMMAND` in docker environment
* `make docker-build-debug` - debug build of the service with all the assertions and sanitizers enabled in docker environment
* `make docker-test-debug` - does a `make build-debug` and runs all the tests on the result in docker environment
* `make docker-start-service-release` - does a `make install-release` and runs service in docker environment
* `make docker-start-service-debug` - does a `make install-debug` and runs service in docker environment
* `make docker-clean-data` - stop docker containers and clean database data

Edit `Makefile.local` to change the default configuration and build options.