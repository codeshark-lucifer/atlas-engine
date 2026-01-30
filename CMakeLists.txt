cmake_minimum_required(VERSION 4.2.1)
project(AtlasEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}   -static -static-libgcc")
endif()

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_VULKAN ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/SDL)

find_package(Vulkan REQUIRED)

set(IMGUI_SRC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
)

set(IMGUI_BACKENDS
    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

file(GLOB_RECURSE ENGINE_SRC
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# if (WIN32)
#     set(APP_ICON ${CMAKE_SOURCE_DIR}/resources/app.rc)
# endif()

add_executable(engine
    ${ENGINE_SRC}
    ${IMGUI_SRC}
    ${IMGUI_BACKENDS}
    # ${APP_ICON}
)

target_link_libraries(engine PRIVATE
    SDL3::SDL3-static
    Vulkan::Vulkan
)

target_include_directories(engine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    external/imgui
    external/imgui/backends
    ${Vulkan_INCLUDE_DIRS}
)

if(WIN32)
    target_link_libraries(engine PRIVATE
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
    # Vulkan requires dynamic linking
    remove_definitions(-static)
endif()
