cmake_minimum_required(VERSION 3.3)
project(clib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# set hadler path
include_directories(include)

# set source path
set(SOURCE_FILES main.c zk-lib/zk_util.h zk-lib/zk_util.c gzip-lib/gzip_util.h gzip-lib/gzip_util.c base64-lib/base64_util.h include/common/common.h base64-lib/base64_util.c lua_utils.c lua_utils.c)

# set lib source
set(SOURCE_LIBS_CLIB zk-lib/zk_util.h zk-lib/zk_util.c gzip-lib/gzip_util.h gzip-lib/gzip_util.c base64-lib/base64_util.h include/common/common.h base64-lib/base64_util.c lua_utils.c)

# set library
link_directories(lib)

# executable .exe
add_executable(clib ${SOURCE_FILES})

add_library(clib_utils SHARED ${SOURCE_LIBS_CLIB})
set_target_properties(clib_utils PROPERTIES VERSION 0.1.0 SOVERSION 0.1.0)
add_library(clib_utils_static STATIC ${SOURCE_LIBS_CLIB})
set_target_properties(clib_utils_static PROPERTIES OUTPUT_NAME "clib_utils")

# link the extarnal lib
target_link_libraries(clib zookeeper_mt z luajit-5.1)