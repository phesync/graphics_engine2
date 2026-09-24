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

add_executable (graphics_engine2 ${SOURCES} ${HEADERS})

target_include_directories(graphics_engine2 PRIVATE
    "C:/Libs/glfw-3.4/include"
    "C:/Libs/glm-1.0.3/glm/include"
    "C:/Libs/glad-3.3/include"
    "src/include"
)

target_link_directories(graphics_engine2 PRIVATE
    "C:/Libs/glfw-3.4/build/src/Release"
)

target_link_libraries(graphics_engine2 PRIVATE
    glfw3
)

add_custom_command(
    TARGET graphics_engine2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:graphics_engine2>/assets"

    COMMENT "Copying asset file."
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET graphics_engine2 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
