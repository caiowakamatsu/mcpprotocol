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

add_library(mcpprotocol
        mcpprotocol/deserializer.cpp
        mcpprotocol/serializer.cpp
)
target_include_directories(mcpprotocol PUBLIC .)
target_compile_options(mcpprotocol PRIVATE
        $<IF:$<CXX_COMPILER_ID:MSVC>,/W4 /WX,-Wall -Wextra -pedantic -Werror>
)

add_executable(tests test/init.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain mcpprotocol)
