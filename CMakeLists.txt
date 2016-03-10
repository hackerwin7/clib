cmake_minimum_required(VERSION 3.3)
project(clib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# set hadler path
include_directories(include)

# set source path
set(SOURCE_FILES main.c zk-lib/zk_util.h zk-lib/zk_util.c gzip-lib/gzip_util.h gzip-lib/gzip_util.c base64-lib/base64_util.h include/common/common.h base64-lib/base64_util.c)

# set lib source
set(SOURCE_LIBS_CLIB ${SOURCE_FILES}
        include/zookeeper
        include/zlib
        include/lua)

# set library search
link_directories(lib)

# generate library
add_library(clib_utils SHARED ${SOURCE_LIBS_CLIB})
add_library(clib_utils_static STATIC ${SOURCE_LIBS_CLIB})
set_target_properties(clib_utils_static PROPERTIES OUTPUT_NAME "clib_utils")

# generate executable
add_executable(clib ${SOURCE_FILES})

# link library
target_link_libraries(clib_utils zookeeper_mt z)
target_link_libraries(clib_utils_static zookeeper_mt z)
target_link_libraries(clib zookeeper_mt z)