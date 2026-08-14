cmake_minimum_required(VERSION 4.3.4)
set(CMAKE_CXX_COMPILER g++)
project(cubic-verification VERSION 0.1.0 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DPP REQUIRED IMPORTED_TARGET dpp)

add_executable(cubic-verification
    src/main.cpp
    src/bot.cpp
    src/command.cpp
    src/localization.cpp
    src/modal.cpp
    src/api.cpp
    src/crypto.cpp
)

target_link_libraries(cubic-verification PRIVATE PkgConfig::DPP)

target_link_libraries(cubic-verification PRIVATE cpr::cpr)

add_custom_command(TARGET cubic-verification POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:cubic-verification>/resources)

add_custom_command(TARGET cubic-verification POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/secrets $<TARGET_FILE_DIR:cubic-verification>/secrets)
