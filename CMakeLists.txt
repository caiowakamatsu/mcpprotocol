cmake_minimum_required(VERSION 3.26)
project(mcpprotocol CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_library(mcpprotocol-lib
        mcpprotocol/serializer.cpp
        mcpprotocol/writer.cpp
        mcpprotocol/reader.cpp
)
target_include_directories(mcpprotocol-lib PUBLIC .)
target_compile_options(mcpprotocol-lib PRIVATE
        $<IF:$<CXX_COMPILER_ID:MSVC>,/W4 /WX,-Wall -Wextra -pedantic -Werror>
)

add_executable(tests
        test/init.cpp
        test/roundtrip.cpp
        test/serializer.cpp
        test/writer.cpp
        test/deserializer.cpp
)
target_link_libraries(tests PRIVATE mcpprotocol-lib Catch2::Catch2WithMain)
