cmake_minimum_required(VERSION 3.26)
project(mcpprotocol CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
set(SOCKPP_BUILD_STATIC ON)
FetchContent_Declare(
        sockpp
        GIT_REPOSITORY https://github.com/fpagliughi/sockpp.git
        GIT_TAG        v1.0.0
)
FetchContent_Declare(
        zlibstatic
        GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng
        GIT_TAG        2.1.6
)
FetchContent_MakeAvailable(Catch2 sockpp zlibstatic)

add_library(mcpprotocol-lib
        mcpprotocol/writer.cpp
        mcpprotocol/reader.cpp
)
target_include_directories(mcpprotocol-lib PUBLIC .)
target_compile_options(mcpprotocol-lib PRIVATE
        $<IF:$<CXX_COMPILER_ID:MSVC>,/W4 /WX,-Wall -Wextra -pedantic -Werror>
)
target_link_libraries(mcpprotocol-lib PRIVATE zlibstatic)

add_executable(tests
        test/init.cpp
        test/roundtrip.cpp
        test/writer.cpp
        test/deserializer.cpp
)
target_link_libraries(tests PRIVATE mcpprotocol-lib Catch2::Catch2WithMain)

add_executable(mcpprotocol-example
        example/main.cpp
)
target_link_libraries(mcpprotocol-example PRIVATE mcpprotocol-lib sockpp-static)
