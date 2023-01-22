set(SRC_FILES server.h
              server.c
              main.c
)

add_executable(server ${SRC_FILES})
target_link_libraries(server PUBLIC pthread utils transport protocol)