cmake_minimum_required(VERSION 3.12)
project(tms_service CXX)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# Adding JWT library
set(JWT_BUILD_TESTS OFF)
set(JWT_BUILD_EXAMPLES OFF)
add_subdirectory(third_party/jwt-cpp)

# Adding Sodium library
find_package(Sodium REQUIRED)

# find_package(ICU 4.4 COMPONENTS io i18n REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc i18n)

# Adding userver dependency
find_package(userver COMPONENTS core postgresql QUIET)
if(NOT userver_FOUND)  # Fallback to subdirectory usage
    # Enable userver libraries that are needed in this project
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)

    # Compatibility mode: some systems don't support these features
    set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
        message(STATUS "Using userver framework from third_party/userver")
        add_subdirectory(third_party/userver)
    else()
        message(FATAL_ERROR "Either install the userver or provide a path to it")
    endif()
endif()

userver_setup_environment()

add_subdirectory(tools)

# Common sources
add_library(${PROJECT_NAME}_objs OBJECT

    # models
    src/models/user.hpp
    src/models/user.cpp
    src/models/task.hpp
    src/models/task.cpp

    # db
    src/db/queries.hpp

    #dto
    src/dto/user.hpp
    src/dto/user.cpp
    src/dto/task.hpp
    src/dto/task.cpp

    # handlers
    src/handlers/details/filter_settings.hpp
    src/handlers/details/filter_settings.cpp

    src/handlers/auth/bearer.hpp
    src/handlers/auth/bearer.cpp

    src/handlers/extra/echo.hpp
    src/handlers/extra/echo.cpp

    src/handlers/users/user_login.hpp
    src/handlers/users/user_login.cpp
    src/handlers/users/user_register.hpp
    src/handlers/users/user_register.cpp

    src/handlers/users/admin/user_create.hpp
    src/handlers/users/admin/user_create.cpp
    src/handlers/users/admin/user_update.hpp
    src/handlers/users/admin/user_update.cpp
    src/handlers/users/admin/user_get.hpp
    src/handlers/users/admin/user_get.cpp
    src/handlers/users/admin/user_delete.hpp
    src/handlers/users/admin/user_delete.cpp

    src/handlers/tasks/task_create.hpp
    src/handlers/tasks/task_create.cpp
    src/handlers/tasks/task_update.hpp
    src/handlers/tasks/task_update.cpp
    src/handlers/tasks/task_get.hpp
    src/handlers/tasks/task_get.cpp
    src/handlers/tasks/task_list_get.hpp
    src/handlers/tasks/task_list_get.cpp
    src/handlers/tasks/task_delete.hpp
    src/handlers/tasks/task_delete.cpp

    src/handlers/tasks/admin/task_create.hpp
    src/handlers/tasks/admin/task_create.cpp
    src/handlers/tasks/admin/task_update.hpp
    src/handlers/tasks/admin/task_update.cpp
    src/handlers/tasks/admin/task_get.hpp
    src/handlers/tasks/admin/task_get.cpp
    src/handlers/tasks/admin/task_list_get.hpp
    src/handlers/tasks/admin/task_list_get.cpp
    src/handlers/tasks/admin/task_delete.hpp
    src/handlers/tasks/admin/task_delete.cpp

    # validators
    src/validators/validation_rules.hpp
    src/validators/validators.hpp
    src/validators/validators.cpp
    src/validators/common_validators.hpp
    src/validators/common_validators.cpp
    src/validators/user_validators.hpp
    src/validators/user_validators.cpp
    src/validators/task_validators.hpp
    src/validators/task_validators.cpp
    src/validators/impl/regex_cache.hpp
    src/validators/impl/regex_cache.cpp

    # utils
    src/utils/jwt.hpp
    src/utils/jwt.cpp
    src/utils/hash.hpp
    src/utils/hash.cpp
    src/utils/error.hpp
    src/utils/error.cpp
    src/utils/string.hpp
)

target_include_directories(
    ${PROJECT_NAME}_objs
    PUBLIC
    ${ICU_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

# find_package(jwt-cpp)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql
                                                  jwt-cpp::jwt-cpp
                                                  sodium
                                                  ${ICU_LIBRARIES})


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
add_executable(${PROJECT_NAME}_unittest
    src/validators/common_validators_test.cpp
    src/validators/user_validators_test.cpp
    src/validators/task_validators_test.cpp
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Benchmarks
add_executable(${PROJECT_NAME}_benchmark
    # src/hello_benchmark.cpp
)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
add_google_benchmark_tests(${PROJECT_NAME}_benchmark)


# Functional Tests
include(UserverTestsuite)

userver_testsuite_add_simple()


# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
