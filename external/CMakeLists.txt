add_subdirectory("glfw")
add_subdirectory("glm")
add_subdirectory("glad")

project(imgui)

add_library(imgui)

target_sources(imgui PRIVATE
"imgui/imgui.cpp"
"imgui/imgui_draw.cpp"
"imgui/imgui_tables.cpp"
"imgui/imgui_widgets.cpp"
"imgui/backends/imgui_impl_opengl3.cpp"
"imgui/backends/imgui_impl_glfw.cpp"
)

target_include_directories(imgui PRIVATE
"glfw/include"
"imgui"
)