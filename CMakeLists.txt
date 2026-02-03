cmake_minimum_required(VERSION 3.26)
project(AtlasEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(OUTPUT_FILE engine)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/ecs)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/stb)

find_package(OpenGL REQUIRED)

set(IMGUI_SRC
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
)

set(IMGUI_BACKENDS
    vendor/imgui/backends/imgui_impl_sdl3.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

file(GLOB_RECURSE ENGINE_SRC
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${OUTPUT_FILE}
    ${ENGINE_SRC}
    ${IMGUI_SRC}
    ${IMGUI_BACKENDS}
)

target_compile_features(${OUTPUT_FILE} PRIVATE cxx_std_20)

set_target_properties(${OUTPUT_FILE} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES OFF
)

target_include_directories(${OUTPUT_FILE} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    vendor/imgui
    vendor/imgui/backends
)

target_link_libraries(${OUTPUT_FILE} PRIVATE
    SDL3::SDL3-static
    ECS
    glad
    stb
)

if(WIN32)
    target_link_libraries(${OUTPUT_FILE} PRIVATE
        winmm imm32 setupapi version
        ole32 oleaut32 uuid advapi32
        shell32 user32 gdi32
    )
endif()

if(MINGW)
    target_compile_options(${OUTPUT_FILE} PRIVATE
        -static -static-libgcc -static-libstdc++
    )
    target_link_options(${OUTPUT_FILE} PRIVATE
        -static -static-libgcc -static-libstdc++
        -Wl,-Bdynamic
    )
endif()
