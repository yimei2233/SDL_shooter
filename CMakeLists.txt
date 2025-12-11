# 标题
cmake_minimum_required(VERSION 3.10.0)
project(SDL_shooter VERSION 0.1.0 LANGUAGES C CXX)

# 设置c++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})
# 目标名称设置
set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# 查找并载入预设
set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})


# 添加SDL2
find_package(SDL2 REQUIRED)
# 添加SDL2_image
# find_package(sdl2_image REQUIRED)
find_library(NAME SDL2_image REQUIRED)
find_library(NAME SDL2_mixer REQUIRED)
find_library(NAME SDL2_ttf REQUIRED)


# 添加可执行文件
add_executable(${TARGET} 
                "./src/main.cpp"
                "./src/Scene_Main.cpp"
                "./src/Game.cpp"
                "./src/SceneTitle.cpp"
                "./src/Scene.cpp"
                )
# 链接对应的库
target_include_directories(${TARGET} PUBLIC ${SDL2_INCLUDE_DIRS})

target_link_libraries(${TARGET}
                        ${SDL2_LIBRARIES}
                        SDL2_image
                        SDL2_mixer
                        SDL2_ttf
                        )