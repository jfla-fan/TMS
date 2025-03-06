cmake_minimum_required(VERSION 3.12)
project(hasher CXX)


find_package(userver REQUIRED COMPONENTS core postgresql)

userver_setup_environment()


add_library(hasher_objs OBJECT

    ${CMAKE_SOURCE_DIR}/src/validators/impl/regex_cache.hpp
    ${CMAKE_SOURCE_DIR}/src/validators/impl/regex_cache.cpp
    ${CMAKE_SOURCE_DIR}/src/validators/common_validators.hpp
    ${CMAKE_SOURCE_DIR}/src/validators/common_validators.cpp
    ${CMAKE_SOURCE_DIR}/src/validators/user_validators.hpp
    ${CMAKE_SOURCE_DIR}/src/validators/user_validators.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/hash.hpp
    ${CMAKE_SOURCE_DIR}/src/utils/hash.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/error.hpp
    ${CMAKE_SOURCE_DIR}/src/utils/error.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/jwt.hpp
    ${CMAKE_SOURCE_DIR}/src/utils/jwt.cpp
    
)

target_compile_definitions(
    hasher_objs
    PRIVATE
    NO_SERVICE
)


target_include_directories(hasher_objs PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${ICU_INCLUDE_DIRS}
)


target_link_libraries(
    hasher_objs
    PUBLIC
    userver::postgresql
    jwt-cpp::jwt-cpp
    sodium
    ${ICU_LIBRARIES}
)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE hasher_objs)