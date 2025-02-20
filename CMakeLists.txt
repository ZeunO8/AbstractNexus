cmake_minimum_required(VERSION 3.22.1)
project(Zeungine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(PkgConfig REQUIRED)

include(CTest)
enable_testing()

message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")

# Set library directory
if(("${CMAKE_BUILD_TYPE}" MATCHES "Release") OR ("${CMAKE_BUILD_TYPE}" MATCHES "MinSizeRel"))
    set(RELEASE_OR_DEBUG "Release")
    set(BUILD_POSTFIX "")
elseif(("${CMAKE_BUILD_TYPE}" MATCHES "Debug") OR ("${CMAKE_BUILD_TYPE}" MATCHES "RelWithDebInfo"))
    set(RELEASE_OR_DEBUG "Debug")
    set(BUILD_POSTFIX "-debug")
endif()
set(ZG_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/${RELEASE_OR_DEBUG}")
message(STATUS "ZG_LIB_DIR: ${ZG_LIB_DIR}")

# Determine platform and set defines
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(LINUX ON)
    set(LIB_SUFFIX ".so")
    if(ZG_INSTALL)
        file(READ "/etc/os-release" OS_RELEASE)
        string(REGEX MATCH "Debian|Ubuntu|Fedora" DIST ${OS_RELEASE})
        if(DIST STREQUAL "Debian" OR
            DIST STREQUAL "Ubuntu")
            set(DEB true)
        elseif(DIST STREQUAL "Fedora")
            set(RPM true)
        else()
            message(FATAL_ERROR ">>>> Found unknown distribution <<<<")
        endif()
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(MACOS ON)
    set(LIB_SUFFIX ".dylib")
elseif(WIN32)
    set(WINDOWS ON)
    set(LIB_SUFFIX ".dll")
endif()

# Add build flags
if(LINUX OR MACOS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -fPIC")
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    endif()
    set(BUILD_SHARED_LIBS ON)
elseif(WINDOWS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4068")
endif()

# Options
option(BUILD_GL "Whether to build OpenGL module" OFF)
option(BUILD_EGL "Whether to build EGL module" OFF)
option(BUILD_VULKAN "Whether to build Vulkan module" ON)
option(BUILD_EDITOR "Whether to build the Zeungine editor" ON)
option(ZG_INSTALL "Whether to install Zeungine" ON)
if(ZG_INSTALL)
    if(LINUX OR MACOS)
        set(ZG_UNX_INSTALL_PREFIX /usr/local)
        set(ZG_SRC_INSTALL_PREFIX ${ZG_UNX_INSTALL_PREFIX}/src/zg)
        set(ZG_LIB_INSTALL_PREFIX ${ZG_UNX_INSTALL_PREFIX}/lib/zg/${CMAKE_BUILD_TYPE})
        set(ZG_INC_INSTALL_PREFIX ${ZG_UNX_INSTALL_PREFIX}/include/zg)
        set(ZG_ASS_INSTALL_PREFIX ${ZG_SRC_INSTALL_PREFIX}/assets)
    elseif(WINDOWS)
        set(ZG_WIN_INSTALL_PREFIX "C:/Program Files/Zeungine")
        set(ZG_SRC_INSTALL_PREFIX ${ZG_WIN_INSTALL_PREFIX})
        set(ZG_LIB_INSTALL_PREFIX ${ZG_WIN_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE})
        set(ZG_INC_INSTALL_PREFIX ${ZG_WIN_INSTALL_PREFIX}/include)
        set(LG_INC_INSTALL_PREFIX "/??/lunasvg") # todo fix quest|ion|
        set(ZG_ASS_INSTALL_PREFIX ${ZG_WIN_INSTALL_PREFIX}/assets)
    endif()
    add_definitions(-DZG_SRC_INSTALL_PREFIX="${ZG_SRC_INSTALL_PREFIX}")
    add_definitions(-DZG_LIB_INSTALL_PREFIX="${ZG_LIB_INSTALL_PREFIX}")
    add_definitions(-DZG_INC_INSTALL_PREFIX="${ZG_INC_INSTALL_PREFIX}")
    add_definitions(-DZG_ASS_INSTALL_PREFIX="${ZG_ASS_INSTALL_PREFIX}")
endif()

# Determine windower
if(LINUX)
    pkg_check_modules(XCB xcb)
    find_package(X11)
    find_package(Wayland)
    option(USE_X11 "Use X11 as a windower" ${X11_FOUND})
    option(USE_XCB "Use XCB as a windower" ${XCB_FOUND})
    option(USE_WAYLAND "Use Wayland as a windower" ${Wayland_FOUND})
elseif(WINDOWS)
    option(USE_WIN32 "Use Windows as a windower" ON)
elseif(MACOS)
    option(USE_MACOS "Use Mac as a windower" ON)
endif()

if(BUILD_VULKAN)
    message(STATUS "Compiling with Vulkan backend")
# elseif(BUILD_GL)
#     message(STATUS "Compiling with OpenGL backend")
#     add_definitions(-DUSE_GL)
# elseif(BUILD_EGL)
#     message(STATUS "Compiling with EGL backend")
#     add_definitions(-DUSE_EGL)
endif()

# Dependencies

# Freetype
set(SKIP_INSTALL_ALL ON)
add_subdirectory(vendor/freetype build/build_freetype)
set_target_properties(freetype PROPERTIES DEBUG_POSTFIX "")
set_target_properties(freetype PROPERTIES RELEASE_POSTFIX "")
set_target_properties(freetype PROPERTIES RELWITHDEBINFO_POSTFIX "")
set_target_properties(freetype PROPERTIES MINSIZEREL_POSTFIX "")
include_directories(vendor/freetype/include)

# BVH
add_subdirectory(vendor/bvh build/build_bvh)
include_directories(vendor/bvh/src)

# GLM
include_directories(vendor/glm)

# STB
include_directories(vendor/stb)

# filewatch
set(BuildTests OFF)
add_subdirectory(vendor/filewatch)
include_directories(vendor/filewatch)

# lunasvg
include(FetchContent)
FetchContent_Declare(
    lunasvg
    GIT_REPOSITORY https://github.com/sammycage/lunasvg.git
    GIT_TAG master
)
FetchContent_MakeAvailable(lunasvg)
include_directories(${lunasvg_SOURCE_DIR}/include)

# Windowing 'USE_X's
if(LINUX)
    if(USE_XCB)
        message(STATUS "Compiling with XCB windowing backend")
        add_definitions(-DUSE_XCB)
        if(NOT USE_XCB)
            set(USE_XCB ON)
        endif()
    endif()
    if(USE_X11)
        message(STATUS "Compiling with X11 windowing backend")
        add_definitions(-DUSE_X11)
        if(NOT USE_X11)
            set(USE_X11 ON)
        endif()
    endif()
    if(USE_WAYLAND)
        message(STATUS "Compiling with Wayland windowing backend")
        add_definitions(-DUSE_WAYLAND)
        if(NOT USE_WAYLAND)
            set(USE_WAYLAND ON)
        endif()
    endif()
elseif(WINDOWS)
    message(STATUS "Compiling with WIN32 windowing backend")
    if(NOT USE_WIN32)
        set(USE_WIN32 ON)
    endif()
    add_definitions(-DUSE_WIN32)
elseif(MACOS)
    message(STATUS "Compiling with MacOS windowing backend")
    if(NOT USE_MACOS)
        set(USE_MACOS ON)
    endif()
    add_definitions(-DUSE_MACOS)
endif()

# Includes
include_directories(include)

# Sources
set(ZG_SOURCES
    src/system/TabulatedIOLogger.cpp
    src/system/TerminalIO.cpp
    src/system/Budget.cpp
    src/Logger.cpp
    src/SharedLibrary.cpp
    src/crypto/vector.cpp
    src/images/ImageLoader.cpp
    src/images/SVGRasterize.cpp
    src/zgfilesystem/File.cpp
    src/zgfilesystem/Directory.cpp
    src/zgfilesystem/DirectoryWatcher.cpp
    src/editor/Hotswapper.cpp
    src/strings/HookedConsole.cpp
    src/strings/InFileProcessor.cpp
    src/interfaces/IPlatformWindow.cpp
    src/system/Command.cpp
    src/Window.cpp
    src/Entity.cpp
    src/Scene.cpp
    src/interfaces/ISizable.cpp
    src/entities/AssetBrowser.cpp
    src/entities/Button.cpp
    src/entities/Console.cpp
    src/entities/Cube.cpp
    src/entities/Dialog.cpp
    src/entities/DropdownMenu.cpp
    src/entities/Input.cpp
    src/entities/Panel.cpp
    src/entities/Plane.cpp
    src/entities/SkyBox.cpp
    src/entities/StatusText.cpp
    src/entities/Tabs.cpp
    src/entities/TextView.cpp
    src/entities/Toolbar.cpp
    src/lights/DirectionalLight.cpp
    src/lights/PointLight.cpp
    src/lights/SpotLight.cpp
    src/shaders/Shader.cpp
    src/shaders/ShaderFactory.cpp
    src/shaders/ShaderManager.cpp
    src/textures/Texture.cpp
    src/textures/TextureFactory.cpp
    src/textures/TextureLoader.cpp
    src/textures/Framebuffer.cpp
    src/textures/FramebufferFactory.cpp
    src/vaos/VAO.cpp
    src/vaos/VAOFactory.cpp
    src/vp/View.cpp
    src/vp/Projection.cpp
    src/vp/VML.cpp
    src/fonts/freetype/Freetype.cpp
    src/raytracing/BVH.cpp)
if(BUILD_GL)
    list(APPEND ZG_SOURCES src/renderers/GLRenderer.cpp src/gl.c)
    if(WIN32)
        list(APPEND ZG_SOURCES src/wgl.c)
    elseif(LINUX)
        list(APPEND ZG_SOURCES src/glx.c)
    elseif(ANDROID OR IOS)
        list(APPEND ZG_SOURCES src/egl.c)
    endif()
elseif(BUILD_EGL)
    list(APPEND ZG_SOURCES src/renderers/EGLRenderer.cpp)
elseif(BUILD_VULKAN)
    list(APPEND ZG_SOURCES src/renderers/VulkanRenderer.cpp)
endif()
if(WIN32)
    list(APPEND ZG_SOURCES src/windows/WIN32Window.cpp)
elseif(LINUX)
    if(USE_X11)
        list(APPEND ZG_SOURCES src/windows/X11Window.cpp)
    endif()
    if(USE_XCB OR USE_X11)
        list(APPEND ZG_SOURCES src/windows/XCBWindow.cpp)
    endif()
    if(USE_WAYLAND)
        list(APPEND ZG_SOURCES
            src/windows/WaylandWindow.cpp
            src/wayland/wayland-xdg-shell-client-protocol.c
            src/wayland/xdg-decoration-unstable-v1-client-protocol.c)
    endif()
elseif(MACOS)
    list(APPEND ZG_SOURCES src/windows/MacOSWindow.mm)
endif()

# Library type, Static if Windows, otherwise Shared
if(WIN32)
    set(ZG_LIBRARY_TYPE STATIC)
else()
    set(ZG_LIBRARY_TYPE SHARED)
endif()

# Add Zeungine library
add_library(zeungine ${ZG_LIBRARY_TYPE} ${ZG_SOURCES})

# Link libraries
if(BUILD_GL)
    if(WIN32)
        find_package(OpenGL REQUIRED)
        target_link_libraries(zeungine OpenGL::GL)
    elseif(LINUX)
        target_link_libraries(zeungine GL)
    endif()
endif()
target_link_libraries(zeungine freetype)
target_link_libraries(zeungine bvh)
if(LINUX)
    if(USE_X11)
        target_link_libraries(zeungine X11 Xrandr Xext Xfixes xkbcommon Xrender)
    endif()
    if(USE_XCB OR USE_X11)
        target_link_libraries(zeungine xcb xkbcommon xcb-util xcb-keysyms xcb-xfixes X11 Xrandr Xext Xfixes xkbcommon Xrender)
    endif()
    if(USE_WAYLAND)
        target_link_libraries(zeungine wayland-client)
    endif()
elseif(MACOS)
    if(BUILD_GL)
        find_package(OpenGL REQUIRED)
        target_link_libraries(zeungine ${OPENGL_LIBRARY})
    endif()
    target_link_libraries(zeungine "-framework Cocoa" "-framework QuartzCore" "-framework Metal")
endif()
if(BUILD_VULKAN)
    find_package(Vulkan REQUIRED)
    include_directories(${Vulkan_INCLUDE_DIRS})
endif()
if(LINUX)
    target_link_libraries(zeungine
        ${ZG_LIB_DIR}/libshaderc_shared.so)
elseif(MACOS)
    target_link_libraries(zeungine
        ${ZG_LIB_DIR}/libshaderc_shared.dylib)
endif()

target_link_libraries(zeungine lunasvg::lunasvg)

# create_test function
function(create_test TEST_NAME TEST_SOURCE TEST_ARGS LIBRARY)
    if(LIBRARY)
        add_library(${TEST_NAME} SHARED ${TEST_SOURCE})
    else()
        add_executable(${TEST_NAME} ${TEST_SOURCE})
    endif()
    target_link_libraries(${TEST_NAME} zeungine)
    if(LINUX)
        target_link_libraries(${TEST_NAME} ${X11_LIBRARIES})
    endif()
    if(NOT LIBRARY)
        add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE_DIR:${TEST_NAME}>/${TEST_NAME}${TEST_EXT} ${TEST_ARGS})
    endif()
endfunction()

# Tests
create_test(LightingTest tests/LightingTest.cpp "" FALSE)
create_test(SimpleCubeTest tests/SimpleCubeTest.cpp "" FALSE)
create_test(SimpleWindowTest tests/SimpleWindowTest.cpp "" FALSE)

# Editor
if(BUILD_EDITOR)
    add_executable(Editor src/editor/Editor.cpp src/editor/EditorScene.cpp src/editor/CodeScene.cpp)
    target_link_libraries(Editor zeungine)
endif()

# Required test assets
set(SKYBOX_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/images/skybox)
file(MAKE_DIRECTORY ${SKYBOX_OUT_DIRECTORY})
configure_file(${CMAKE_SOURCE_DIR}/images/skybox/back.jpg ${SKYBOX_OUT_DIRECTORY}/back.jpg COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/images/skybox/front.jpg ${SKYBOX_OUT_DIRECTORY}/front.jpg COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/images/skybox/left.jpg ${SKYBOX_OUT_DIRECTORY}/left.jpg COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/images/skybox/right.jpg ${SKYBOX_OUT_DIRECTORY}/right.jpg COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/images/skybox/top.jpg ${SKYBOX_OUT_DIRECTORY}/top.jpg COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/images/skybox/bottom.jpg ${SKYBOX_OUT_DIRECTORY}/bottom.jpg COPYONLY)
set(FONTS_DIRECTORY ${CMAKE_BINARY_DIR}/fonts)
set(IMAGES_DIRECTORY ${CMAKE_BINARY_DIR}/images)
file(COPY ${CMAKE_SOURCE_DIR}/fonts/Roboto/Roboto-Regular.ttf DESTINATION ${FONTS_DIRECTORY}/Roboto)
file(COPY ${CMAKE_SOURCE_DIR}/images/zeungine-icon.png DESTINATION ${IMAGES_DIRECTORY})
file(COPY ${ZG_LIB_DIR}/libvk_swiftshader${LIB_SUFFIX} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/icons DESTINATION ${CMAKE_BINARY_DIR})

# Install
if(ZG_INSTALL)
    install(DIRECTORY include/angle DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY include/GL DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY include/glad DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY include/shaderc DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY include/SwiftShader DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY include/wayland DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY include/zg DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY vendor/glm/glm DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(DIRECTORY vendor/freetype/include/freetype DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    install(FILES
        vendor/freetype/include/ft2build.h
        DESTINATION ${ZG_INC_INSTALL_PREFIX}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    install(DIRECTORY vendor/bvh/src/bvh DESTINATION ${ZG_INC_INSTALL_PREFIX}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    if(LINUX)
        execute_process(COMMAND whoami
            OUTPUT_VARIABLE CURRENT_USER
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(INC_CHOWN_COMMAND "sudo chown -R ${CURRENT_USER}:${CURRENT_USER} ${ZG_INC_INSTALL_PREFIX}")
        install(CODE "
            execute_process(COMMAND ${INC_CHOWN_COMMAND})
        ")
    endif()
    install(TARGETS zeungine
        ARCHIVE DESTINATION ${ZG_LIB_INSTALL_PREFIX}
        LIBRARY DESTINATION ${ZG_LIB_INSTALL_PREFIX})

    set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_LIST_DIR}/releases")
    if(LINUX OR MACOS)
        set(CPACK_PACKAGE_NAME "libzeungine${BUILD_POSTFIX}")
    elseif(WINDOWS)
        set(CPACK_PACKAGE_NAME "zeungine${BUILD_POSTFIX}")
    endif()
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Zeungine: an engine allowing developers to build cross platform games apps and tools")
    set(CPACK_PACKAGE_DESCRIPTION "Contains various structs and methods to help build cross platform games apps and tools quickly")
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://zeungine.dev")
    if(LINUX OR MACOS)
        # set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_LIST_DIR}/images/zeungine.dev-app-icon.bmp")
    elseif(WINDOWS)
        # set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_LIST_DIR}\\\\images\\\\zeungine.dev-app-icon.bmp")
    endif()
    set(CPACK_PACKAGE_VENDOR "zeungine.dev")
    set(CPACK_PACKAGE_CONTACT "zeun@zeungine.dev")

    if(DEB)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Zeun")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgl1-mesa-dri,libx11-6,libvulkan1,libxrandr2,libxfixes3,libxkbcommon-x11-0,ttf-mscorefonts-installer,libcurl4-gnutls-dev,libsecret-1-0,libglib2.0-dev")
    elseif(WINDOWS)
        set(CPACK_GENERATOR "NSIS")
        set(CPACK_CMAKE_GENERATOR "Ninja")
        set(CPACK_NSIS_MODIFY_PATH ON)
        set(CPACK_NSIS_MENU_LINKS "https://zeungine.dev" "Zeungine Web Site")
        set(CPACK_NSIS_DISPLAY_NAME Zeungine)
        set(CPACK_NSIS_PACKAGE_NAME Zeungine)
        SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Program Files\\\\Zeungine")
        SET(CPACK_NSIS_INSTALL_ROOT "C:")
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/LICENSE")
        # set(CPACK_NSIS_MUI_ICON "${Zeungine_SRC_DIR}\\\\images\\\\zeungine-logo-new.ico")
        set(CPACK_NSIS_UNINSTALL_NAME "UninstallZeungine")
        set(CPACK_NSIS_BRANDING_TEXT " ")
    endif()

    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    include(CPack)
endif()
