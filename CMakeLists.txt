cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME netcheck_bypass)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

include_directories(
)

link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(${SHORT_NAME}
  netcheck_bypass.c
)

target_link_libraries(${SHORT_NAME}
  taihen_stub
)

set_target_properties(${SHORT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(${SHORT_NAME}.suprx ${SHORT_NAME}
  CONFIG ${CMAKE_SOURCE_DIR}/${SHORT_NAME}.yml
)
