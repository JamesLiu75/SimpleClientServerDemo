set(SRC_FILES transport.h
               network/linux/network.h
               network/linux/network.c
)

add_library(transport ${SRC_FILES})
target_include_directories(transport PUBLIC .)
target_include_directories(transport PUBLIC network/linux)
target_link_libraries(transport PUBLIC utils)