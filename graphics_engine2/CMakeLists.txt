# CMakeList.txt : CMake project for graphics_engine2, include source and define
# project specific logic here.
#

# Add source to this project's executable.

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.c"
)

file(GLOB_RECURSE HEADERS
    "src/*.h"
)

target_sources (graphics_engine2 PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(graphics_engine2 PRIVATE
    "src/include"
)

# TODO: Add tests and install targets if needed.
