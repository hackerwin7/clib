cmake_minimum_required(VERSION 3.3)
project(clib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# set hadler path
include_directories(include common)

# set source path
set(SOURCE_FILES main.c zk-lib/zk_util.h zk-lib/zk_util.c gzip-lib/gzip_util.h gzip-lib/gzip_util.c base64-lib/base64_util.h
        base64-lib/base64_util.c protobuf/ngmsg.pb-c.h protobuf/ngmsg.pb-c.c protobuf/ngmsg_util.h protobuf/ngmsg_util.c common/common.h common/common.c common/gc_ring_buffer.h common/gc_ring_buffer.c)

# set lib source
set(SOURCE_LIBS_CLIB ${SOURCE_FILES}
        include/zookeeper
        include/zlib
        include/lua
        include/protobuf-c)

# set library search
link_directories(lib)

# generate library
add_library(clib_utils SHARED ${SOURCE_LIBS_CLIB})
add_library(clib_utils_static STATIC ${SOURCE_LIBS_CLIB})
set_target_properties(clib_utils_static PROPERTIES OUTPUT_NAME "clib_utils")

# generate executable
add_executable(clib ${SOURCE_FILES})

# link library
target_link_libraries(clib_utils
        "-Wl,--whole-archive"
        protobuf-c zookeeper_mt z
        "-Wl,--no-whole-archive")
target_link_libraries(clib_utils_static
        "-Wl,--whole-archive"
        protobuf-c zookeeper_mt z
        "-Wl,--no-whole-archive")
target_link_libraries(clib protobuf-c zookeeper_mt z)