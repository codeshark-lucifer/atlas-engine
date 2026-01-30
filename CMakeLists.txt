cmake_minimum_required(VERSION 3.26)

set(OUTPUT_FILE engine)
project(${OUTPUT_FILE} VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(MINGW)
    add_compile_options(-static -static-libgcc -static-libstdc++)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_VULKAN ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/SDL)

find_package(Vulkan REQUIRED)

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

target_include_directories(${OUTPUT_FILE} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    vendor/imgui
    vendor/imgui/backends
)

target_link_libraries(${OUTPUT_FILE} PRIVATE
    SDL3::SDL3-static
    Vulkan::Vulkan
)

if(WIN32)
    target_link_libraries(${OUTPUT_FILE} PRIVATE
        winmm
        imm32
        setupapi
        version
        ole32
        oleaut32
        uuid
        advapi32
        shell32
        user32
        gdi32
    )
endif()

if(MINGW)
    target_link_options(${OUTPUT_FILE} PRIVATE -Wl,-Bdynamic)
endif()
