set(SRC_FILES protocol.h
              protocol.c
)

add_library(protocol ${SRC_FILES})
target_include_directories(protocol PUBLIC .)