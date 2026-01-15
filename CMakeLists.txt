cmake_minimum_required(VERSION 3.8)
project(CCTG_Sim_Drones VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

option(BUILD_WEB "build for web" OFF)

set(CMAKE_BUILD_TYPE Release)

if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSCRIPTEN_FLAGS_STRING}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS_STRING}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_FLAGS_STRING}")
endif ()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS  "src/*.cpp")

set(CMAKE_CXX_STANDARD 20)  # or 20 for C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Optional: disable compiler-specific extensions


add_executable(CCTG_Sim_Drones ${SOURCES} "src/Game.h" "src/Game.cpp" "src/utils.h" "src/utils.cpp" "src/globals.h" "src/Button.h" "src/Button.cpp" "src/Cell.h" "src/Cell.cpp"  "src/Pathfinder.cpp" "src/Pathfinder.h" "src/Drone.h" "src/Drone.cpp" "src/Raycast.h" "src/Raycast.cpp")
target_include_directories(CCTG_Sim_Drones PRIVATE "C:\\Users\\ethan\\Documents\\Work\\Programming\\Other\\Raylib\\build\\raylib\\include")


if(${BUILD_WEB})
    target_compile_definitions(CCTG_Sim_Drones PRIVATE PLATFORM_WEB=1)
    target_link_directories(CCTG_Sim_Drones PRIVATE "C:\\Users\\ethan\\Documents\\Work\\Programming\\Other\\Raylib\\build\\raylib")
    target_include_directories(CCTG_Sim_Drones PRIVATE "C:\\Users\\ethan\\Documents\\Work\\Programming\\Other\\emscripten\\upstream\\emscripten\\cache\\sysroot\\include")
else()
    target_link_directories(CCTG_Sim_Drones PRIVATE "C:\\Users\\ethan\\Documents\\Work\\Programming\\Other\\raylib-static-release")
endif()

target_link_libraries(CCTG_Sim_Drones PRIVATE raylib)
target_link_libraries(CCTG_Sim_Drones PRIVATE Winmm)
target_include_directories(CCTG_Sim_Drones PRIVATE ${nlohmann_json_SOURCE_DIR}/include)
