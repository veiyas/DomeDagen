cmake_minimum_required(VERSION 3.10 FATAL_ERROR)


#
#
# @TODO: Change this project to the name of your project
#
#
project("Domedagen")

#
# Add the libraries that this application depends on,
#  1. SGCT (https://github.com/opensgct/sgct) for window handling and cluster support
#  2. libwebsockets (https://libwebsockets.org) for support for WebSockets
#  3. FBXSDK for importing models, textures, animation, etc

# Disable SSL requirement as it requires another library
set(LWS_WITH_SSL OFF CACHE BOOL "" FORCE)
set(LWS_WITHOUT_TESTAPPS ON CACHE BOOL "" FORCE)
add_subdirectory(ext/libwebsockets)
set_property(TARGET dist PROPERTY FOLDER "Dependencies")
set_property(TARGET websockets PROPERTY FOLDER "Dependencies")
set_property(TARGET websockets_shared PROPERTY FOLDER "Dependencies")

add_subdirectory(ext/sgct)
set_property(TARGET sgct PROPERTY FOLDER "Dependencies")

add_subdirectory(ext/assimp)
set_property(TARGET assimp PROPERTY FOLDER "Dependencies")


#
# Adding the source files here that are compiled for this project
#
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/websockethandler.h
  src/websockethandler.cpp
  src/utility.hpp
  src/renderable.hpp
  src/gameobject.hpp
  src/gameobject.cpp
  src/player.hpp
  src/player.cpp
  src/game.hpp
  src/game.cpp
  src/shaders/playervert.glsl
  src/shaders/playerfrag.glsl
  src/shaders/testingvert.glsl
  src/shaders/testingfrag.glsl
)
target_include_directories(${PROJECT_NAME} PRIVATE
  src
  ext/sgct/include
  ext/libwebsockets/include
  ext/fbxsdk/include
  ${LIBWEBSOCKETS_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE sgct websockets libfbxsdk)


#
# Setting some compile settings for the project
#
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

if (MSVC)
  # Microsoft Visual Studio related compile options
  target_compile_options(${PROJECT_NAME} PRIVATE
    "/ZI"       # Edit and continue support
    "/MP"       # Multi-threading support
    "/W4"       # Highest warning level
    "/wd4201"   # nonstandard extension used : nameless struct/union    
    "/std:c++17"
    "/permissive-"
    "/Zc:strictStrings-"    # Windows header don't adhere to this
    "/Zc:__cplusplus" # Correctly set the __cplusplus macro
  )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # When compiling on Clang.  This most likely means compiling on MacOS
  target_compile_options(${PROJECT_NAME} PRIVATE
    "-stdlib=libc++"
    "-Wall"
    "-Wextra"
  )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # Probably compiling on Linux
  target_compile_options(${PROJECT_NAME} PRIVATE
    "-ggdb"
    "-Wall"
    "-Wextra"
    "-Wpedantic"
  )
endif ()
