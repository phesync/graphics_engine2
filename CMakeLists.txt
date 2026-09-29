# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("graphics_engine2")

add_executable (graphics_engine2)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET graphics_engine2 PROPERTY CXX_STANDARD 20)
endif()

add_custom_command(
    TARGET graphics_engine2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/graphics_engine2/assets"
            "$<TARGET_FILE_DIR:graphics_engine2>/assets"

    COMMENT "Copying asset file."
)

# Include sub-projects.
add_subdirectory ("graphics_engine2")

add_subdirectory("external")

target_link_libraries(graphics_engine2 PRIVATE glfw)
target_link_libraries(graphics_engine2 PRIVATE glm)
target_link_libraries(graphics_engine2 PRIVATE glad)

target_link_libraries(graphics_engine2 PRIVATE imgui)

target_include_directories(graphics_engine2 PRIVATE
"external/glad/include"
"external/glfw/include",
"external/glm",
"external/imgui",
"external/imgui/backends"
)