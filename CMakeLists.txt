cmake_minimum_required(VERSION 2.8)

project(craft)

set(DEBUG 0)
# Unfortunately, the Vulkan backend has an crashing bug when you reset (outer
# main loop). Best guess is that it's missing cleanup or bad initialization.
# Until such a time, we'll keep on defaulting to GL.
set(VULKANIZE 0)

FILE(GLOB SOURCE_FILES src/*.c)
FILE(GLOB VERTEX_SHADERS shaders/*_vertex.glsl)
FILE(GLOB FRAGMENT_SHADERS shaders/*_fragment.glsl)

if(VULKANIZE)
    LIST(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/glrenderer.c)
else(VULKANIZE)
    LIST(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/vkrenderer.c)
endif(VULKANIZE)

foreach(VERT ${VERTEX_SHADERS})
  get_filename_component(BASE_NAME ${VERT} NAME_WLE)
  set(SPV "shaders/${BASE_NAME}.spv")
  add_custom_command(
        OUTPUT ${SPV}
        COMMAND glslangValidator -V -S vert ${VERT} -o ${SPV}
        DEPENDS ${VERT}
  )
  list(APPEND SPIRS_V ${SPV})
endforeach(VERT)

foreach(FRAG ${FRAGMENT_SHADERS})
  get_filename_component(BASE_NAME ${FRAG} NAME_WLE)
  set(SPV "shaders/${BASE_NAME}.spv")
  add_custom_command(
        OUTPUT ${SPV}
        COMMAND glslangValidator -V -S frag ${FRAG} -o ${SPV}
        DEPENDS ${FRAG}
  )
  list(APPEND SPIRS_V ${SPV})
endforeach(FRAG)

add_custom_target(
        Shaders
        DEPENDS ${SPIRS_V}
)

add_executable(
    craft
    ${SOURCE_FILES}
    deps/lodepng/lodepng.c
    deps/noise/noise.c
    deps/sqlite/sqlite3.c
    deps/tinycthread/tinycthread.c)

add_library(world SHARED
    src/world.c
    deps/noise/noise.c)

if(DEBUG)
add_definitions(-g -ggdb -O0)
else(DEBUG)
add_definitions(-O3)
endif(DEBUG)

if(VULKANIZE)
add_definitions(-std=c99 -Wall -DVULKANIZE=1 -DGLFW_INCLUDE_VULKAN)
else(VULKANIZE)
add_definitions(-std=c99 -Wall -DVULKANIZE=0)
endif(VULKANIZE)

add_dependencies(craft
     Shaders
)

include_directories(deps/lodepng)
include_directories(deps/noise)
include_directories(deps/sqlite)
include_directories(deps/tinycthread)

if(MINGW)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}
        "C:/Program Files/CURL/lib" "C:/Program Files (x86)/CURL/lib")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH}
        "C:/Program Files/CURL/include" "C:/Program Files (x86)/CURL/include")
endif()

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

if(VULKANIZE)
    set(GRAPHICS_LIB vulkan)
else(VULKANIZE)
    if(APPLE)
        set(GRAPHICS_LIB GLEW::GLEW)
    else(APPLE)
        set(GRAPHICS_LIB GLEW)
    endif(APPLE)
endif(VULKANIZE)

if(APPLE)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)

    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)

    target_link_libraries(craft glfw m OpenGL::GL ${GRAPHICS_LIB}
        ${GLFW_LIBRARIES} ${CURL_LIBRARIES})
endif(APPLE)

if(UNIX)
    target_link_libraries(craft dl glfw m GL ${GRAPHICS_LIB}
        ${GLFW_LIBRARIES} ${CURL_LIBRARIES})
endif()

if(MINGW)
    target_link_libraries(craft ws2_32.lib glfw m GL ${GRAPHICS_LIB}
        ${GLFW_LIBRARIES} ${CURL_LIBRARIES})
endif()
