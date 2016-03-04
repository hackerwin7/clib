cmake_minimum_required(VERSION 3.3)
project(clib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# set hadler path
include_directories(include)

# set source path
set(SOURCE_FILES main.c zk-lib/zk_util.h zk-lib/zk_util.c gzip-lib/gzip_util.h gzip-lib/gzip_util.c base64-lib/base64_util.h include/common/common.h)

# set library
link_directories(lib)

add_executable(clib ${SOURCE_FILES})

target_link_libraries(clib zookeeper_mt z)