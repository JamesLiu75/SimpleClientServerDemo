set(SRC_FILES client.h
              client.c
              main.c
)

add_executable(client ${SRC_FILES})
target_include_directories(client PUBLIC .)
target_link_libraries(client PUBLIC utils transport protocol)