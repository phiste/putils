set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/CMakeModules)
include(cmake_helper)
include(conan_helper)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()

file(GLOB src *.cpp *.hpp)
add_library(putils STATIC ${src})
target_include_directories(putils PUBLIC .)
target_include_directories(putils PUBLIC reflection)

add_subdirectory(pluginManager)
target_link_libraries(putils PUBLIC putils_pluginManager)

set(imguiSrc imgui/imgui.cpp imgui/imgui_widgets.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp)
add_library(putils_imgui STATIC ${imguiSrc})
target_include_directories(putils_imgui PUBLIC imgui)
target_link_libraries(putils PUBLIC putils_imgui)

if(PUTILS_BUILD_LUA)
    add_subdirectory(lua)
    target_link_libraries(putils PUBLIC putils_lua)

    file(GLOB imgui_lua_src
        imgui_lua_bindings/*.cpp
        imgui_lua_bindings/*.hpp)
    add_library(putils_imgui_lua_bindings STATIC ${imgui_lua_src})
    target_link_libraries(putils_imgui_lua_bindings PUBLIC putils)
    target_link_libraries(putils PUBLIC putils_imgui_lua_bindings)
endif()

if(PUTILS_BUILD_MEDIATOR)
    add_subdirectory(mediator)
    target_link_libraries(putils PUBLIC putils_mediator)
    target_compile_definitions(putils PUBLIC PUTILS_MEDIATOR)
endif()

if(PUTILS_BUILD_PYTHON)
    add_subdirectory(python)
    target_link_libraries(putils PUBLIC putils_python)
    target_compile_definitions(putils PUBLIC PUTILS_PYTHON)
endif()

if (PUTILS_BUILD_OPENGL)
    add_subdirectory(opengl)
    target_link_libraries(putils PUBLIC putils_opengl)
    target_compile_definitions(putils PUBLIC PUTILS_OPENGL)
endif ()

if (PUTILS_BUILD_VULKAN)
    add_subdirectory(vulkan)
    target_link_libraries(putils PUBLIC putils_vulkan)
    target_compile_definitions(putils PUBLIC PUTILS_VULKAN)
endif()

if (PUTILS_NO_SHADER_DEBUG)
    target_compile_definitions(putils PUBLIC PUTILS_NO_SHADER_DEBUG)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
