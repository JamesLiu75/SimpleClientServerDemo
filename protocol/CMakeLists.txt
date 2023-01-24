set(SRC_FILES protocol.h
              protocol.c
)

add_library(protocol ${SRC_FILES})
target_compile_options(protocol PRIVATE -Wall -Walloc-zero -Wextra -Wpedantic -Werror)
target_include_directories(protocol PUBLIC .)