set(SRC_FILES log.c
               log.h
               utils.h
               utils.c
)

add_library(utils ${SRC_FILES})
target_include_directories(utils PUBLIC .)