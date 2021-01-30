# Add all cpp and h files
file(GLOB SRC_FILES
    "*.h"
    "*.c"
)

add_executable(homework1 ${SRC_FILES})