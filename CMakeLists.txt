cmake_minimum_required(VERSION 4.2)

project(async_msg 
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(server 
    apps/server_main.cpp
    src/server.cpp
    src/protocol.cpp
)

target_include_directories(server
  PRIVATE
     include
)

add_executable(protocol_test
     tests/protocol_test.cpp
     src/protocol.cpp
)

target_include_directories(protocol_test
   PRIVATE
     include
)

add_executable(protocol_client
tests/protocol_client.cpp
src/protocol.cpp
)

target_include_directories(protocol_client
PRIVATE
  include
)

add_executable(server_class_test
tests/server_class_test.cpp
src/server_class.cpp
src/protocol.cpp
)

target_include_directories(server_class_test
PRIVATE
include
)
