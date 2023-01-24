cmake_minimum_required (VERSION 2.8.11)
project (SimpleClientServer)

include (CTest)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Walloc-zero  -Wnonnull -Wformat-overflow=2 -Wextra -Wpedantic -Werror -fsanitize=address)
add_link_options(-fsanitize=address)

find_program( BASH_PROGRAM bash)

add_subdirectory(utilities)
add_subdirectory(transport)
add_subdirectory(protocol)
add_subdirectory(server)
add_subdirectory(client)

if (BASH_PROGRAM)
add_test(
    NAME final_test
    COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/test/test.sh
)
endif(BASH_PROGRAM)